/***************************************************************************
  testqgsnetworkanalysis.cpp
  --------------------------
Date                 : November 2017
Copyright            : (C) 2017 by Nyall Dawson
Email                : nyall dot dawson at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "qgstest.h"

//header for class being tested
#include "qgsgeometrysnapper.h"
#include "qgsgeometry.h"
#include <qgsapplication.h>
#include "qgsvectordataprovider.h"
#include "qgsvectorlayer.h"
#include "qgsvectorlayerdirector.h"
#include "qgsnetworkdistancestrategy.h"
#include "qgsgraphbuilder.h"
#include "qgsgraph.h"
#include "qgsgraphanalyzer.h"

class TestQgsNetworkAnalysis : public QObject
{
    Q_OBJECT

  public:

  private slots:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase();// will be called after the last testfunction was executed.
    void init() ;// will be called before each testfunction is executed.
    void cleanup() ;// will be called after every testfunction.
    void testGraph();
    void testBuild();
    void testBuildTolerance();
    void dijkkjkjkskkjsktra();

  private:
    std::unique_ptr< QgsVectorLayer > buildNetwork();


};

class TestNetworkStrategy : public QgsNetworkStrategy
{
    QSet< int > requiredAttributes() const override
    {
      return QSet< int >() << 0;
    }
    QVariant cost( double, const QgsFeature &f ) const override
    {
      return f.attribute( 0 );
    }
};

void  TestQgsNetworkAnalysis::initTestCase()
{
  //
  // Runs once before any tests are run
  //
  // init QGIS's paths - true means that all path will be inited from prefix
  QgsApplication::init();
  QgsApplication::initQgis();
}
void  TestQgsNetworkAnalysis::cleanupTestCase()
{
  QgsApplication::exitQgis();
}
void  TestQgsNetworkAnalysis::init()
{

}
void  TestQgsNetworkAnalysis::cleanup()
{

}

void TestQgsNetworkAnalysis::testGraph()
{
  QgsGraph graph;
  QCOMPARE( graph.vertexCount(), 0 );
  QCOMPARE( graph.edgeCount(), 0 );
  graph.addVertex( QgsPointXY( 1, 2 ) );
  QCOMPARE( graph.vertexCount(), 1 );
  QCOMPARE( graph.vertex( 0 ).point(), QgsPointXY( 1, 2 ) );
  QVERIFY( graph.vertex( 0 ).outgoingEdges().empty() );
  QVERIFY( graph.vertex( 0 ).incomingEdges().empty() );
  QCOMPARE( graph.findVertex( QgsPointXY( 1, 2 ) ), 0 );
  graph.addVertex( QgsPointXY( 3, 4 ) );
  QCOMPARE( graph.vertexCount(), 2 );
  QCOMPARE( graph.vertex( 1 ).point(), QgsPointXY( 3, 4 ) );
  QVERIFY( graph.vertex( 1 ).outgoingEdges().empty() );
  QVERIFY( graph.vertex( 1 ).incomingEdges().empty() );
  QCOMPARE( graph.findVertex( QgsPointXY( 1, 2 ) ), 0 );
  QCOMPARE( graph.findVertex( QgsPointXY( 3, 4 ) ), 1 );
  QCOMPARE( graph.edgeCount(), 0 );

  graph.addEdge( 0, 1, QVector< QVariant >() << 9 );
  QCOMPARE( graph.edgeCount(), 1 );
  QCOMPARE( graph.edge( 0 ).cost( 0 ).toInt(), 9 );
  QCOMPARE( graph.edge( 0 ).fromVertex(), 0 );
  QCOMPARE( graph.edge( 0 ).toVertex(), 1 );
  QCOMPARE( graph.vertex( 0 ).incomingEdges(), QList< int >() );
  QCOMPARE( graph.vertex( 0 ).outgoingEdges(), QList< int >() << 0 );
  QCOMPARE( graph.vertex( 1 ).incomingEdges(), QList< int >() << 0 );
  QCOMPARE( graph.vertex( 1 ).outgoingEdges(), QList< int >() );

  graph.addVertex( QgsPointXY( 7, 8 ) );
  QCOMPARE( graph.vertexCount(), 3 );
  QCOMPARE( graph.vertex( 2 ).point(), QgsPointXY( 7, 8 ) );
  QVERIFY( graph.vertex( 2 ).outgoingEdges().empty() );
  QVERIFY( graph.vertex( 2 ).incomingEdges().empty() );
  QCOMPARE( graph.edgeCount(), 1 );
  graph.addEdge( 1, 2, QVector< QVariant >() << 8 );

  QCOMPARE( graph.edge( 1 ).cost( 0 ).toInt(), 8 );
  QCOMPARE( graph.edge( 1 ).fromVertex(), 1 );
  QCOMPARE( graph.edge( 1 ).toVertex(), 2 );
  QCOMPARE( graph.vertex( 1 ).incomingEdges(), QList< int >() << 0 );
  QCOMPARE( graph.vertex( 1 ).outgoingEdges(), QList< int >() << 1 );
  QCOMPARE( graph.vertex( 2 ).incomingEdges(), QList< int >() << 1 );
  QCOMPARE( graph.vertex( 2 ).outgoingEdges(), QList< int >() );
}

std::unique_ptr<QgsVectorLayer> TestQgsNetworkAnalysis::buildNetwork()
{
  std::unique_ptr< QgsVectorLayer > l = qgis::make_unique< QgsVectorLayer >( QStringLiteral( "LineString?crs=epsg:4326&field=cost:int" ), QStringLiteral( "x" ), QStringLiteral( "memory" ) );

  QgsFeature ff( 0 );
  QgsGeometry refGeom = QgsGeometry::fromWkt( QStringLiteral( "LineString(0 0, 10 0, 10 10)" ) );
  ff.setGeometry( refGeom );
  ff.setAttributes( QgsAttributes() << 1 );
  QgsFeatureList flist;
  flist << ff;
  l->dataProvider()->addFeatures( flist );

  return l;
}


void TestQgsNetworkAnalysis::testBuild()
{
  std::unique_ptr<QgsVectorLayer> network = buildNetwork();
  std::unique_ptr< QgsVectorLayerDirector > director = qgis::make_unique< QgsVectorLayerDirector > ( network.get(),
      -1, QString(), QString(), QString(), QgsVectorLayerDirector::DirectionBoth );
  std::unique_ptr< QgsNetworkDistanceStrategy > strategy = qgis::make_unique< QgsNetworkDistanceStrategy >();
  director->addStrategy( strategy.release() );
  std::unique_ptr< QgsGraphBuilder > builder = qgis::make_unique< QgsGraphBuilder > ( network->sourceCrs(), true, 0 );

  QVector<QgsPointXY > snapped;
  director->makeGraph( builder.get(), QVector<QgsPointXY>() << QgsPointXY( 0, 0 ) << QgsPointXY( 10, 10 ), snapped );
  QCOMPARE( snapped, QVector<QgsPointXY>() << QgsPointXY( 0, 0 ) << QgsPointXY( 10, 10 ) );
  std::unique_ptr< QgsGraph > graph( builder->graph() );
  QCOMPARE( graph->vertexCount(), 3 );
  QCOMPARE( graph->edgeCount(), 4 );
  QCOMPARE( graph->vertex( 0 ).point(), QgsPointXY( 0, 0 ) );
  QCOMPARE( graph->vertex( 0 ).outgoingEdges(), QList< int >() << 0 );
  QCOMPARE( graph->edge( 0 ).fromVertex(), 0 );
  QCOMPARE( graph->edge( 0 ).toVertex(), 1 );
  QCOMPARE( graph->vertex( 0 ).incomingEdges(), QList< int >()  << 1 );
  QCOMPARE( graph->edge( 1 ).fromVertex(), 1 );
  QCOMPARE( graph->edge( 1 ).toVertex(), 0 );
  QCOMPARE( graph->vertex( 1 ).point(), QgsPointXY( 10, 0 ) );
  QCOMPARE( graph->vertex( 1 ).outgoingEdges(), QList< int >() << 1 << 2 );
  QCOMPARE( graph->vertex( 1 ).incomingEdges(), QList< int >() << 0 << 3 );
  QCOMPARE( graph->edge( 3 ).fromVertex(), 2 );
  QCOMPARE( graph->edge( 3 ).toVertex(), 1 );
  QCOMPARE( graph->edge( 2 ).fromVertex(), 1 );
  QCOMPARE( graph->edge( 2 ).toVertex(), 2 );
  QCOMPARE( graph->vertex( 2 ).point(), QgsPointXY( 10, 10 ) );
  QCOMPARE( graph->vertex( 2 ).outgoingEdges(), QList< int >() << 3 );
  QCOMPARE( graph->vertex( 2 ).incomingEdges(), QList< int >() << 2 );

  builder = qgis::make_unique< QgsGraphBuilder > ( network->sourceCrs(), true, 0 );
  director->makeGraph( builder.get(), QVector<QgsPointXY>() << QgsPointXY( 10, 0 ) << QgsPointXY( 10, 10 ), snapped );
  QCOMPARE( snapped, QVector<QgsPointXY>() << QgsPointXY( 10, 0 ) << QgsPointXY( 10, 10 ) );

  builder = qgis::make_unique< QgsGraphBuilder > ( network->sourceCrs(), true, 0 );
  director->makeGraph( builder.get(), QVector<QgsPointXY>(), snapped );
  QCOMPARE( snapped, QVector<QgsPointXY>() );

  builder = qgis::make_unique< QgsGraphBuilder > ( network->sourceCrs(), true, 0 );
  director->makeGraph( builder.get(), QVector<QgsPointXY>() << QgsPointXY( 0.2, 0.1 ) << QgsPointXY( 10.1, 9 ), snapped );
  QCOMPARE( snapped, QVector<QgsPointXY>() << QgsPointXY( 0.2, 0.0 ) << QgsPointXY( 10.0, 9 ) );
  graph.reset( builder->graph() );
  QCOMPARE( graph->vertexCount(), 5 );
  QCOMPARE( graph->edgeCount(), 8 );

}

void TestQgsNetworkAnalysis::testBuildTolerance()
{
  std::unique_ptr<QgsVectorLayer> network = buildNetwork();
  // has already a linestring LineString(0 0, 10 0, 10 10)

  QgsFeature ff( 0 );
  // 0.1 distance gap
  QgsGeometry refGeom = QgsGeometry::fromWkt( QStringLiteral( "LineString(10.1 10, 20 10 )" ) );
  ff.setGeometry( refGeom );
  QgsFeatureList flist;
  flist << ff;
  network->dataProvider()->addFeatures( flist );

  std::unique_ptr< QgsVectorLayerDirector > director = qgis::make_unique< QgsVectorLayerDirector > ( network.get(),
      -1, QString(), QString(), QString(), QgsVectorLayerDirector::DirectionBoth );
  std::unique_ptr< QgsNetworkDistanceStrategy > strategy = qgis::make_unique< QgsNetworkDistanceStrategy >();
  director->addStrategy( strategy.release() );
  std::unique_ptr< QgsGraphBuilder > builder = qgis::make_unique< QgsGraphBuilder > ( network->sourceCrs(), true, 0 );

  QVector<QgsPointXY > snapped;
  director->makeGraph( builder.get(), QVector<QgsPointXY>(), snapped );
  std::unique_ptr< QgsGraph > graph( builder->graph() );
  QCOMPARE( graph->vertexCount(), 5 );
  QCOMPARE( graph->edgeCount(), 6 );
  QCOMPARE( graph->vertex( 0 ).point(), QgsPointXY( 0, 0 ) );
  QCOMPARE( graph->vertex( 0 ).outgoingEdges(), QList< int >() << 0 );
  QCOMPARE( graph->edge( 0 ).fromVertex(), 0 );
  QCOMPARE( graph->edge( 0 ).toVertex(), 1 );
  QCOMPARE( graph->vertex( 0 ).incomingEdges(), QList< int >()  << 1 );
  QCOMPARE( graph->edge( 1 ).fromVertex(), 1 );
  QCOMPARE( graph->edge( 1 ).toVertex(), 0 );
  QCOMPARE( graph->vertex( 1 ).point(), QgsPointXY( 10, 0 ) );
  QCOMPARE( graph->vertex( 1 ).outgoingEdges(), QList< int >() << 1 << 2 );
  QCOMPARE( graph->vertex( 1 ).incomingEdges(), QList< int >() << 0 << 3 );
  QCOMPARE( graph->edge( 3 ).fromVertex(), 2 );
  QCOMPARE( graph->edge( 3 ).toVertex(), 1 );
  QCOMPARE( graph->edge( 2 ).fromVertex(), 1 );
  QCOMPARE( graph->edge( 2 ).toVertex(), 2 );
  QCOMPARE( graph->vertex( 2 ).point(), QgsPointXY( 10, 10 ) );
  QCOMPARE( graph->vertex( 2 ).outgoingEdges(), QList< int >() << 3 );
  QCOMPARE( graph->vertex( 2 ).incomingEdges(), QList< int >() << 2 );
  QCOMPARE( graph->vertex( 3 ).point(), QgsPointXY( 10.1, 10 ) );
  QCOMPARE( graph->vertex( 3 ).outgoingEdges(), QList< int >() << 4 );
  QCOMPARE( graph->vertex( 3 ).incomingEdges(), QList< int >() << 5 );
  QCOMPARE( graph->vertex( 4 ).point(), QgsPointXY( 20, 10 ) );
  QCOMPARE( graph->edge( 5 ).fromVertex(), 4 );
  QCOMPARE( graph->edge( 5 ).toVertex(), 3 );
  QCOMPARE( graph->edge( 4 ).fromVertex(), 3 );
  QCOMPARE( graph->edge( 4 ).toVertex(), 4 );

  // with tolerance
  double tolerance = 0.11;

  builder = qgis::make_unique< QgsGraphBuilder > ( network->sourceCrs(), true, tolerance );
  director->makeGraph( builder.get(), QVector<QgsPointXY>(), snapped );
  graph.reset( builder->graph() );
  QCOMPARE( graph->vertexCount(), 4 );
  QCOMPARE( graph->edgeCount(), 6 );
  QCOMPARE( graph->vertex( 0 ).point(), QgsPointXY( 0, 0 ) );
  QCOMPARE( graph->vertex( 0 ).outgoingEdges(), QList< int >() << 0 );
  QCOMPARE( graph->edge( 0 ).fromVertex(), 0 );
  QCOMPARE( graph->edge( 0 ).toVertex(), 1 );
  QCOMPARE( graph->vertex( 0 ).incomingEdges(), QList< int >()  << 1 );
  QCOMPARE( graph->edge( 1 ).fromVertex(), 1 );
  QCOMPARE( graph->edge( 1 ).toVertex(), 0 );
  QCOMPARE( graph->vertex( 1 ).point(), QgsPointXY( 10, 0 ) );
  QCOMPARE( graph->vertex( 1 ).outgoingEdges(), QList< int >() << 1 << 2 );
  QCOMPARE( graph->vertex( 1 ).incomingEdges(), QList< int >() << 0 << 3 );
  QCOMPARE( graph->edge( 3 ).fromVertex(), 2 );
  QCOMPARE( graph->edge( 3 ).toVertex(), 1 );
  QCOMPARE( graph->edge( 2 ).fromVertex(), 1 );
  QCOMPARE( graph->edge( 2 ).toVertex(), 2 );
  QCOMPARE( graph->vertex( 2 ).point(), QgsPointXY( 10, 10 ) );
  QCOMPARE( graph->vertex( 2 ).outgoingEdges(), QList< int >() << 3 << 4 );
  QCOMPARE( graph->vertex( 2 ).incomingEdges(), QList< int >() << 2 << 5 );
  QCOMPARE( graph->vertex( 3 ).point(), QgsPointXY( 20, 10 ) );
  QCOMPARE( graph->vertex( 3 ).outgoingEdges(), QList< int >() << 5 );
  QCOMPARE( graph->vertex( 3 ).incomingEdges(), QList< int >() << 4 );
  QCOMPARE( graph->edge( 5 ).fromVertex(), 3 );
  QCOMPARE( graph->edge( 5 ).toVertex(), 2 );
  QCOMPARE( graph->edge( 4 ).fromVertex(), 2 );
  QCOMPARE( graph->edge( 4 ).toVertex(), 3 );
}

void TestQgsNetworkAnalysis::dijkkjkjkskkjsktra()
{
  std::unique_ptr<QgsVectorLayer> network = buildNetwork();
  // has already a linestring LineString(0 0, 10 0, 10 10)

  // add some more lines to network
  QgsFeature ff( 0 );
  QgsFeatureList flist;

  ff.setGeometry( QgsGeometry::fromWkt( QStringLiteral( "LineString(10 10, 20 10 )" ) ) );
  ff.setAttributes( QgsAttributes() << 2 );
  flist << ff;
  ff.setGeometry( QgsGeometry::fromWkt( QStringLiteral( "LineString(10 20, 10 10 )" ) ) );
  ff.setAttributes( QgsAttributes() << 3 );
  flist << ff;
  ff.setGeometry( QgsGeometry::fromWkt( QStringLiteral( "LineString(20 -10, 20 10 )" ) ) );
  ff.setAttributes( QgsAttributes() << 4 );
  flist << ff;
  network->dataProvider()->addFeatures( flist );

  /*
   Out network is:
   Numbers in brackets are cost for segment

  20           o
               |
               v (3)
               |    (2)
  10           o---->----o
               |         |
          (1)  ^         ^
         (1)   |         |
  0 o---->-----.         |(4)
                         |
                         ^
                         |
  -10                    o

    0          10        20

  */

  // build graph
  std::unique_ptr< QgsVectorLayerDirector > director = qgis::make_unique< QgsVectorLayerDirector > ( network.get(),
      -1, QString(), QString(), QString(), QgsVectorLayerDirector::DirectionBoth );
  std::unique_ptr< QgsNetworkStrategy > strategy = qgis::make_unique< TestNetworkStrategy >();
  director->addStrategy( strategy.release() );
  std::unique_ptr< QgsGraphBuilder > builder = qgis::make_unique< QgsGraphBuilder > ( network->sourceCrs(), true, 0 );

  QVector<QgsPointXY > snapped;
  director->makeGraph( builder.get(), QVector<QgsPointXY>(), snapped );
  std::unique_ptr< QgsGraph > graph( builder->graph() );

  int startVertexIdx = graph->findVertex( QgsPointXY( 20, -10 ) );
  QVERIFY( startVertexIdx != -1 );

  // both directions
  QVector<int> resultTree;
  QVector<double> resultCost;
  QgsGraphAnalyzer::dijkstra( graph.get(), startVertexIdx, 0, &resultTree, &resultCost );

  int point_0_0_idx = graph->findVertex( QgsPointXY( 0, 0 ) );
  QVERIFY( point_0_0_idx != -1 );
  int point_10_0_idx = graph->findVertex( QgsPointXY( 10, 0 ) );
  QVERIFY( point_10_0_idx != -1 );
  int point_10_10_idx = graph->findVertex( QgsPointXY( 10, 10 ) );
  QVERIFY( point_10_10_idx != -1 );
  int point_10_20_idx = graph->findVertex( QgsPointXY( 10, 20 ) );
  QVERIFY( point_10_20_idx != -1 );
  int point_20_10_idx = graph->findVertex( QgsPointXY( 20, 10 ) );
  QVERIFY( point_20_10_idx != -1 );

  QCOMPARE( resultTree.at( startVertexIdx ), -1 );
  QCOMPARE( resultCost.at( startVertexIdx ), 0.0 );

  QVERIFY( resultTree.at( point_20_10_idx ) != -1 );
  QCOMPARE( resultCost.at( point_20_10_idx ), 4.0 );
  QCOMPARE( graph->edge( resultTree.at( point_20_10_idx ) ).fromVertex(), startVertexIdx );
  QCOMPARE( graph->edge( resultTree.at( point_20_10_idx ) ).toVertex(), point_20_10_idx );
  QVERIFY( resultTree.at( point_10_10_idx ) != -1 );
  QCOMPARE( resultCost.at( point_10_10_idx ), 6.0 );
  QCOMPARE( graph->edge( resultTree.at( point_10_10_idx ) ).fromVertex(), point_20_10_idx );
  QCOMPARE( graph->edge( resultTree.at( point_10_10_idx ) ).toVertex(), point_10_10_idx );
  QVERIFY( resultTree.at( point_10_20_idx ) != -1 );
  QCOMPARE( resultCost.at( point_10_20_idx ), 9.0 );
  QCOMPARE( graph->edge( resultTree.at( point_10_20_idx ) ).fromVertex(), point_10_10_idx );
  QCOMPARE( graph->edge( resultTree.at( point_10_20_idx ) ).toVertex(), point_10_20_idx );
  QVERIFY( resultTree.at( point_10_0_idx ) != -1 );
  QCOMPARE( resultCost.at( point_10_0_idx ), 7.0 );
  QCOMPARE( graph->edge( resultTree.at( point_10_0_idx ) ).fromVertex(), point_10_10_idx );
  QCOMPARE( graph->edge( resultTree.at( point_10_0_idx ) ).toVertex(), point_10_0_idx );
  QVERIFY( resultTree.at( point_0_0_idx ) != -1 );
  QCOMPARE( resultCost.at( point_0_0_idx ), 8.0 );
  QCOMPARE( graph->edge( resultTree.at( point_0_0_idx ) ).fromVertex(), point_10_0_idx );
  QCOMPARE( graph->edge( resultTree.at( point_0_0_idx ) ).toVertex(), point_0_0_idx );

  // forward direction
  director = qgis::make_unique< QgsVectorLayerDirector > ( network.get(),
             -1, QString(), QString(), QString(), QgsVectorLayerDirector::DirectionForward );
  strategy = qgis::make_unique< TestNetworkStrategy >();
  director->addStrategy( strategy.release() );
  builder = qgis::make_unique< QgsGraphBuilder > ( network->sourceCrs(), true, 0 );
  director->makeGraph( builder.get(), QVector<QgsPointXY>(), snapped );
  graph.reset( builder->graph() );
  startVertexIdx = graph->findVertex( QgsPointXY( 0, 0 ) );
  QVERIFY( startVertexIdx != -1 );
  resultTree.clear();
  resultCost.clear();
  QgsGraphAnalyzer::dijkstra( graph.get(), startVertexIdx, 0, &resultTree, &resultCost );
  point_0_0_idx = graph->findVertex( QgsPointXY( 0, 0 ) );
  QVERIFY( point_0_0_idx != -1 );
  point_10_0_idx = graph->findVertex( QgsPointXY( 10, 0 ) );
  QVERIFY( point_10_0_idx != -1 );
  point_10_10_idx = graph->findVertex( QgsPointXY( 10, 10 ) );
  QVERIFY( point_10_10_idx != -1 );
  point_10_20_idx = graph->findVertex( QgsPointXY( 10, 20 ) );
  QVERIFY( point_10_20_idx != -1 );
  point_20_10_idx = graph->findVertex( QgsPointXY( 20, 10 ) );
  QVERIFY( point_20_10_idx != -1 );
  int point_20_n10_idx = graph->findVertex( QgsPointXY( 20, -10 ) );
  QVERIFY( point_20_n10_idx != -1 );

  QCOMPARE( resultTree.at( startVertexIdx ), -1 );
  QCOMPARE( resultCost.at( startVertexIdx ), 0.0 );
  QVERIFY( resultTree.at( point_10_0_idx ) != -1 );
  QCOMPARE( resultCost.at( point_10_0_idx ), 1.0 );
  QCOMPARE( graph->edge( resultTree.at( point_10_0_idx ) ).fromVertex(), startVertexIdx );
  QCOMPARE( graph->edge( resultTree.at( point_10_0_idx ) ).toVertex(), point_10_0_idx );
  QVERIFY( resultTree.at( point_10_10_idx ) != -1 );
  QCOMPARE( resultCost.at( point_10_10_idx ), 2.0 );
  QCOMPARE( graph->edge( resultTree.at( point_10_10_idx ) ).fromVertex(), point_10_0_idx );
  QCOMPARE( graph->edge( resultTree.at( point_10_10_idx ) ).toVertex(), point_10_10_idx );
  QCOMPARE( resultTree.at( point_10_20_idx ), -1 ); // unreachable
  QVERIFY( resultTree.at( point_20_10_idx ) != -1 );
  QCOMPARE( resultCost.at( point_20_10_idx ), 4.0 );
  QCOMPARE( graph->edge( resultTree.at( point_20_10_idx ) ).fromVertex(), point_10_10_idx );
  QCOMPARE( graph->edge( resultTree.at( point_20_10_idx ) ).toVertex(), point_20_10_idx );
  QCOMPARE( resultTree.at( point_20_n10_idx ),  -1 ); // unreachable

  // backward direction
  director = qgis::make_unique< QgsVectorLayerDirector > ( network.get(),
             -1, QString(), QString(), QString(), QgsVectorLayerDirector::DirectionBackward );
  strategy = qgis::make_unique< TestNetworkStrategy >();
  director->addStrategy( strategy.release() );
  builder = qgis::make_unique< QgsGraphBuilder > ( network->sourceCrs(), true, 0 );
  director->makeGraph( builder.get(), QVector<QgsPointXY>(), snapped );
  graph.reset( builder->graph() );
  startVertexIdx = graph->findVertex( QgsPointXY( 10, 10 ) );
  QVERIFY( startVertexIdx != -1 );
  resultTree.clear();
  resultCost.clear();
  QgsGraphAnalyzer::dijkstra( graph.get(), startVertexIdx, 0, &resultTree, &resultCost );
  point_0_0_idx = graph->findVertex( QgsPointXY( 0, 0 ) );
  QVERIFY( point_0_0_idx != -1 );
  point_10_0_idx = graph->findVertex( QgsPointXY( 10, 0 ) );
  QVERIFY( point_10_0_idx != -1 );
  point_10_10_idx = graph->findVertex( QgsPointXY( 10, 10 ) );
  QVERIFY( point_10_10_idx != -1 );
  point_10_20_idx = graph->findVertex( QgsPointXY( 10, 20 ) );
  QVERIFY( point_10_20_idx != -1 );
  point_20_10_idx = graph->findVertex( QgsPointXY( 20, 10 ) );
  QVERIFY( point_20_10_idx != -1 );
  point_20_n10_idx = graph->findVertex( QgsPointXY( 20, -10 ) );
  QVERIFY( point_20_n10_idx != -1 );

  QCOMPARE( resultTree.at( startVertexIdx ), -1 );
  QCOMPARE( resultCost.at( startVertexIdx ), 0.0 );
  QCOMPARE( resultTree.at( point_20_10_idx ), -1 );
  QCOMPARE( resultTree.at( point_20_n10_idx ), -1 );
  QVERIFY( resultTree.at( point_10_20_idx ) != -1 );
  QCOMPARE( resultCost.at( point_10_20_idx ), 3.0 );
  QCOMPARE( graph->edge( resultTree.at( point_10_20_idx ) ).fromVertex(), startVertexIdx );
  QCOMPARE( graph->edge( resultTree.at( point_10_20_idx ) ).toVertex(), point_10_20_idx );
  QVERIFY( resultTree.at( point_10_0_idx ) != -1 );
  QCOMPARE( resultCost.at( point_10_0_idx ), 1.0 );
  QCOMPARE( graph->edge( resultTree.at( point_10_0_idx ) ).fromVertex(), startVertexIdx );
  QCOMPARE( graph->edge( resultTree.at( point_10_0_idx ) ).toVertex(), point_10_0_idx );
  QVERIFY( resultTree.at( point_0_0_idx ) != -1 );
  QCOMPARE( resultCost.at( point_0_0_idx ), 2.0 );
  QCOMPARE( graph->edge( resultTree.at( point_0_0_idx ) ).fromVertex(), point_10_0_idx );
  QCOMPARE( graph->edge( resultTree.at( point_0_0_idx ) ).toVertex(), point_0_0_idx );
}



QGSTEST_MAIN( TestQgsNetworkAnalysis )
#include "testqgsnetworkanalysis.moc"
