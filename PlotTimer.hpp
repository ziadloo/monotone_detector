//
// Created by mehran on 2022-02-10.
//

#ifndef QWT_GIT_MYTIMER_H
#define QWT_GIT_MYTIMER_H

#include <QwtPlot>
#include <QwtPlotCurve>
#include <QwtPlotGrid>
#include <QwtSymbol>
#include <QwtLegend>
#include <qwt_plot_item.h>

#include <QDebug>
#include <QTimer>
#include <cstdlib>
#include <vector>
#include <mutex>
#include "utility.h"
#include <iostream>

using namespace std;

class PlotTimer : public QObject {
    Q_OBJECT
public:
    PlotTimer() {
        plot = new QwtPlot();
        plot->setCanvasBackground( Qt::white );
        plot->setAxisScale( QwtAxis::XBottom, 0.0, static_cast<double>(_SAMPLE_RATE_/2) );
        plot->setAxisScale( QwtAxis::YLeft, _PLOT_MIN_Y_, _PLOT_MAX_Y_ );
        plot->insertLegend( new QwtLegend() );
        plot->resize( 1280, 768 );

        x.resize(_NUM_POINTS_/2);
        spectrum.resize(_NUM_POINTS_/2);
        similarity.resize(_NUM_POINTS_/2);
        for (int i=0; i<_NUM_POINTS_/2; i++) {
            x[i] = static_cast<double>(i) / (_NUM_POINTS_/2.0) * (_SAMPLE_RATE_/2.0);
            spectrum[i] = 0.0;
            similarity[i] = 0.0;
        }

        timer = new QTimer(this);

        // setup signal and slot
        connect(timer, SIGNAL(timeout()), this, SLOT(PlotTimerSlot()));
        // msec
        timer->start(1000 / _FRAMES_PER_SEC_);
    }
    void set_vectors(vector<double> spectrum, vector<double> similarity, vector<double> noises) {
        this->guard.lock();
        this->spectrum = spectrum;
        this->similarity = similarity;
        this->bars = noises;
        this->guard.unlock();
    }
    void show() {
        plot->show();
    }

    QTimer *timer;
    QwtPlot *plot;
    vector<double> x;
    vector<double> spectrum;
    vector<double> similarity;
    vector<double> bars;
    std::mutex guard;
    int frame_index = 0;
    int frame_index_reference = 0;
    int frame_counter;


public Q_SLOTS:
    void PlotTimerSlot() {
        vector<double> _spectrum;
        vector<double> _similarity;

        guard.lock();
        _spectrum = spectrum;
        _similarity = similarity;
        guard.unlock();


        plot->detachItems();

        QwtPlotGrid* grid = new QwtPlotGrid();
        grid->attach( plot );

        for (auto b : bars) {
            vector<double> _x(2);
            vector<double> _y(2);

            _x[0] = b;
            _y[0] = _PLOT_MIN_Y_;
            _x[1] = b;
            _y[1] = _PLOT_MAX_Y_;

            QwtPlotCurve* curve = new QwtPlotCurve();
            curve->setItemAttribute(QwtPlotItem::Legend, false);
            curve->setPen(Qt::red, 4);
            curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);

            curve->setSamples(&_x[0], &_y[0], 2);
            curve->attach(plot);
        }

        {
            QwtPlotCurve *curve = new QwtPlotCurve();
            curve->setItemAttribute(QwtPlotItem::Legend, false);
            curve->setPen(Qt::green, 2);
            curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);

            curve->setSamples(&x[0], &_spectrum[0], _NUM_POINTS_ / 2);
            curve->attach(plot);
        }

        {
            for (int i=0; i<_similarity.size(); i++) {
                _similarity[i] = _similarity[i] * 100.0;
            }

            QwtPlotCurve* curve = new QwtPlotCurve();
            curve->setItemAttribute(QwtPlotItem::Legend, false);
            curve->setPen(Qt::blue, 2);
            curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);

            curve->setSamples(&x[0], &_similarity[0], _NUM_POINTS_ / 2);
            curve->attach(plot);
        }

        plot->replot();
    }
};


#endif //QWT_GIT_MYTIMER_H
