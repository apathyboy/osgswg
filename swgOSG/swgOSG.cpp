/** -*-c++-*-
 *  \file   swgOSG.cpp
 *  \author Kenneth R. Sewell III

 Visualization of SWG data files.
 Copyright (C) 2009 Kenneth R. Sewell III

 swgOSG is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 swgOSG is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with swgOSG; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <iostream>
#include <memory>
#include <map>
#include <string>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LOD>
#include <osg/Material>
#include <osg/Math>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/Texture2D>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/DriveManipulator>

#include <osgDB/Registry>
#include <osgDB/ReaderWriter>

#include <swgRepository.hpp>

osg::ref_ptr<osg::Node> buildTerrain()
{
  // Create new geode to store geometry.
  osg::ref_ptr< osg::Geode > geode( new osg::Geode() );

  osg::Vec3Array* vertices = new osg::Vec3Array;
  vertices->push_back( osg::Vec3( -12000.0, 0.0, -12000.0 ) );
  vertices->push_back( osg::Vec3( -12000.0, 0.0, 12000.0 ) );
  vertices->push_back( osg::Vec3( 12000.0, 0.0, -12000.0 ) );
  vertices->push_back( osg::Vec3( 12000.0, 0.0, 12000.0 ) );

  osg::Vec3Array* normals = new osg::Vec3Array;
  normals->push_back( osg::Vec3( 0.0, -1.0, 0.0 ) );
  normals->push_back( osg::Vec3( 0.0, -1.0, 0.0 ) );
  normals->push_back( osg::Vec3( 0.0, -1.0, 0.0 ) );
  normals->push_back( osg::Vec3( 0.0, -1.0, 0.0 ) );

  // Create new geometry node list of vertex attributes.
  osg::Geometry* geometry = new osg::Geometry;
  geometry->setVertexArray( vertices );
  geometry->setNormalArray( normals );
  geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX );

  osg::DrawElementsUShort* drawElements =
    new osg::DrawElementsUShort( osg::PrimitiveSet::TRIANGLES );
  drawElements->reserve( 6 );
  drawElements->push_back( 0 );
  drawElements->push_back( 1 );
  drawElements->push_back( 2 );
  drawElements->push_back( 2 );
  drawElements->push_back( 3 );
  drawElements->push_back( 1 );

  // Add primitive set to this geometry node.
  geometry->addPrimitiveSet( drawElements );
  geometry->setUseDisplayList( false );
  geode->addDrawable( geometry );

  return geode;
}

int main( int argc, char **argv )
{
  std::cout << "argc: " << argc << std::endl;

  if( 3 > argc )
    {
      std::cout << "Usage: " << argv[0]
		<< " <directory containing .tre files> "
		<< " <path/to/file/in/tre/archive> "
		<< std::endl;
      return 0;
    }

  std::string treDirectory( argv[1] );
  for( unsigned int i = 0; i < treDirectory.size(); ++i )
    {
      if( treDirectory[i] == '\\' )
        {
          treDirectory[i] = '/';
        }
    }

  if( *(treDirectory.rbegin()) != '/' )
    {
      treDirectory.push_back( '/' );
    }

  swgRepository repo( treDirectory );

  osg::ref_ptr<osg::MatrixTransform> rootNode( new osg::MatrixTransform );

  rootNode->setMatrix(
		      osg::Matrix::rotate(
					  osg::DegreesToRadians( 90.0 ),
					  1.0, 0.0, 0.0 )
		      );

  try {

  unsigned int numFiles = ( argc - 2 );
  for( unsigned int i = 0; i < numFiles; ++i )
    {
      std::string filename( argv[2+i] );
      rootNode->addChild( repo.loadFile( filename ) );
    }
  } catch (std::exception& exception) {
	  exit(0);
  }

  // construct the viewer.
  osgViewer::Viewer viewer;

  // add model to viewer.
  viewer.setSceneData( rootNode.get() );

  //viewer.get

  viewer.setUpViewInWindow( 20, 20, 640, 480 );
  //viewer.setUpViewAcrossAllScreens();

  viewer.addEventHandler( new osgViewer::ScreenCaptureHandler );
  viewer.addEventHandler( new osgViewer::LODScaleHandler );
  viewer.addEventHandler( new osgViewer::StatsHandler );

  osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator
    = new osgGA::KeySwitchMatrixManipulator;
  
  osg::ref_ptr<osgGA::TrackballManipulator>
    tb = new osgGA::TrackballManipulator();

  keyswitchManipulator->addMatrixManipulator('1',
					     "Trackball",
					     tb.get()
					     );
  keyswitchManipulator->addMatrixManipulator( '2',
					      "Flight",
					      new osgGA::FlightManipulator()
					      );
  keyswitchManipulator->addMatrixManipulator( '3',
					      "Drive",
					      new osgGA::DriveManipulator()
					      );
  keyswitchManipulator->addMatrixManipulator( '4',
					      "Terrain",
					      new osgGA::TerrainManipulator()
					      );

  viewer.setCameraManipulator( keyswitchManipulator.get() );

  return viewer.run();
}
