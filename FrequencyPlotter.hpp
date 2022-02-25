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

class FrequencyPlotter : public QObject {
    Q_OBJECT
public:
    FrequencyPlotter() {
        plot = new QwtPlot();
        plot->setCanvasBackground( Qt::white );
        plot->setAxisScale( QwtAxis::XBottom, 0.0, static_cast<double>(_SAMPLE_RATE_/2) );
        plot->setAxisScale( QwtAxis::YLeft, _PLOT_MIN_Y_, _PLOT_MAX_Y_ );
        plot->insertLegend( new QwtLegend() );
        plot->resize( 1280, 768 );

        x.resize(_NUM_POINTS_/2);
        for (int i=0; i<_NUM_POINTS_/2; i++) {
            x[i] = static_cast<double>(i) / (_NUM_POINTS_/2.0) * (_SAMPLE_RATE_/2.0);
        }
    }
    void show() {
        plot->show();
    }

public slots:
    void set_vectors(vector_double spectrum, vector_double similarity, vector_double noises) {
        plot->detachItems();

        QwtPlotGrid* grid = new QwtPlotGrid();
        grid->attach( plot );

        for (auto b : noises) {
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

            curve->setSamples(&x[0], &spectrum[0], _NUM_POINTS_ / 2);
            curve->attach(plot);
        }

        {
            for (int i=0; i<similarity.size(); i++) {
                similarity[i] = similarity[i] * 100.0;
            }

            QwtPlotCurve* curve = new QwtPlotCurve();
            curve->setItemAttribute(QwtPlotItem::Legend, false);
            curve->setPen(Qt::blue, 2);
            curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);

            curve->setSamples(&x[0], &similarity[0], _NUM_POINTS_ / 2);
            curve->attach(plot);
        }

        plot->replot();
    }

private:
    QwtPlot *plot;
    vector<double> x;
};


#endif //QWT_GIT_MYTIMER_H
