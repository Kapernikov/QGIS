/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/gui/raster/qgspalettedrendererwidget.h                           *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/






class QgsPalettedRendererWidget: QgsRasterRendererWidget
{

%TypeHeaderCode
#include "qgspalettedrendererwidget.h"
%End
  public:

    QgsPalettedRendererWidget( QgsRasterLayer *layer, const QgsRectangle &extent = QgsRectangle() );
    ~QgsPalettedRendererWidget();
    static QgsRasterRendererWidget *create( QgsRasterLayer *layer, const QgsRectangle &extent ) /Factory/;
%Docstring
 :rtype: QgsRasterRendererWidget
%End

    virtual QgsRasterRenderer *renderer();


    void setFromRenderer( const QgsRasterRenderer *r );

};

/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/gui/raster/qgspalettedrendererwidget.h                           *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/
