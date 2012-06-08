/** -*-c++-*-
 *  \file   swgRepository.cpp
 *  \author Kenneth R. Sewell III

 Visualization of SWG data files.
 Copyright (C) 2009 Kenneth R. Sewell III

 This file is part of swgOSG.

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

#include <swgRepository.hpp>
#include <meshLib/apt.hpp>
#include <meshLib/cmp.hpp>
#include <meshLib/cshd.hpp>
#include <meshLib/ilf.hpp>
#include <meshLib/lod.hpp>
#include <meshLib/msh.hpp>
#include <meshLib/mlod.hpp>
#include <meshLib/prto.hpp>
#include <meshLib/sbot.hpp>
#include <meshLib/sht.hpp>
#include <meshLib/skmg.hpp>
#include <meshLib/sktm.hpp>
#include <meshLib/stat.hpp>
#include <meshLib/stot.hpp>
#include <meshLib/swts.hpp>
#include <meshLib/trn.hpp>
#include <meshLib/ws.hpp>

#include <memory>

#include <osgDB/Registry>
#include <osg/Point>
#include <osg/ShapeDrawable>

#include <osgText/Text>

swgRepository::swgRepository( const std::string &archiveFilePath )
{
  createArchive( archiveFilePath );

  // Get pointer to ddsplugin.
  ddsPlugin = osgDB::Registry::instance()->getReaderWriterForExtension( "dds" );
}

swgRepository::~swgRepository()
{
}

osg::ref_ptr< osg::Node >
swgRepository::loadFile( const std::string &filename )
{
  if( filename.empty() )
    {
      std::cout << "loadFile called with null filename!" << std::endl;
      return NULL;
    }
  
  // See if file is already loaded...
  std::map< std::string, osg::ref_ptr< osg::Node > >::iterator
    currentFile = nodeMap.find( filename );
  
  // If file was found return ref_ptr to it.
  if( nodeMap.end() != currentFile )
    {
      // File has already been loaded.
      std::cout << "File already loaded: " << filename << std::endl;
      return currentFile->second;
    }
  
  // Otherwise we need to read the file from the archive.
  std::cout << "Reading file from archive: " << filename << std::endl;

  // Read file data into a stream
  std::auto_ptr< std::istream >
    iffFile( archive.getFileStream( filename ) );
  
  if( NULL == iffFile.get() )
    {
      std::cout << "Unable to find file in archive!" << std::endl;
      return NULL;
    }

  // Figure out what type this generic .iff actually is.
  std::string type =  ml::base::getType( *iffFile );
  
  osg::ref_ptr< osg::Node > newNode = NULL;
  if( "APT " == type )
    {
      newNode = loadAPT( iffFile );
    }
  else if( "CCLT" == type )
    {
      ; // Do nothing
    }
  else if( "CMPA" == type )
    {
      newNode = loadCMP( iffFile );
    }
  else if( "DTLA" == type )
    {
      newNode = loadLOD( iffFile );
    }
  else if( "INLY" == type )
    {
      newNode = loadINLY( iffFile );
    }
  else if( "MESH" == type )
    {
      newNode = loadMSH( iffFile );
    }
  else if( "MLOD" == type )
    {
      newNode = loadMLOD( iffFile );
    }
  else if( "PEFT" == type )
    {
      ; // Do nothing
    }
  else if( "PRTO" == type )
    {
      newNode = loadPRTO( iffFile );
    }
  else if( "PTAT" == type )
    {
      newNode = loadTRN( iffFile );
    }
  else if( "SBOT" == type )
    {
      newNode = loadSBOT( iffFile );
    }
  else if( "STAT" == type )
    {
      newNode = loadSTAT( iffFile );
    }
  else if( "STOT" == type )
    {
      newNode = loadSTOT( iffFile );
    }
  else if( "WSNP" == type )
    {
      newNode = loadWSNP( iffFile );
    }
  else if( "SKMG" == type )
    {
      newNode = loadSKMG( iffFile );
    }
  else if( "SKTM" == type )
    {
      newNode = loadSKTM( iffFile );
    }
  
  if( NULL != newNode )
    {
      nodeMap[filename] = newNode;
    }
  
  return newNode;
}

osg::ref_ptr< osg::Texture2D >
swgRepository::loadTextureFile( const std::string &filename )
{
  if( filename.empty() )
    {
      std::cout << "loadTextureFile called with null filename!" << std::endl;
      return NULL;
    }
  
  // See if file is already loaded...
  std::map< std::string, osg::ref_ptr< osg::Texture2D > >::iterator
    currentTexture = textureMap.find( filename );
  
  // If file was found return ref_ptr to it.
  if( textureMap.end() != currentTexture )
    {
      // File has already been loaded.
      std::cout << "Texture file already loaded: " << filename << std::endl;
      return currentTexture->second;
    }
  
  // Otherwise we need to read the file from the archive.
  std::cout << "Reading file from archive: " << filename << std::endl;

  // Setup an auto_ptr to handle the istream.
  std::auto_ptr<std::istream>
    textureFile( archive.getFileStream( filename ) );
  
  // Call DDS plugin directly to read from istream.
  if( !ddsPlugin )
    {
      std::cout << "DDS plugin failed to load." << std::endl;
      return NULL;
    }

  osgDB::ReaderWriter::ReadResult
    result = ddsPlugin->readImage( *textureFile );
  
  // If plugin was successful, then create new texture with
  // dds file.
  osg::ref_ptr<osg::Texture2D> texture( NULL );
  if( result.status() == osgDB::ReaderWriter::ReadResult::FILE_LOADED )
    {
      std::cout << "Loaded texture: " << filename << std::endl;
      texture = new osg::Texture2D;
      texture->setImage( result.getImage() );
    }
  
  if( NULL != texture )
    {
      textureMap[filename] = texture;
    }

  return texture;
}

osg::ref_ptr< osg::StateSet >
swgRepository::loadShader( const std::string &shaderFilename )
{
  if( shaderFilename.empty() )
    {
      std::cout << "loadShader called with null filename!" << std::endl;
      return NULL;
    }

  std::map< std::string, osg::ref_ptr< osg::StateSet > >::iterator
    currentState = stateMap.find( shaderFilename );

  if( stateMap.end() != currentState )
    {
      // Shader has already been loaded.
      std::cout << "Shader already loaded: " << shaderFilename << std::endl;
      return currentState->second;
    }

  // Otherwise we need to read the shader from the archive.
  std::cout << "Reading shader from archive: " << shaderFilename << std::endl;

  // Read file data into a stream
  std::auto_ptr< std::istream >
    shaderFile( archive.getFileStream( shaderFilename ) );
  
  // Figure out what type this generic .iff actually is.
  std::string type =  ml::base::getType( *shaderFile );
  
  if( "SSHT" != type
      && "CSHD" != type
      && "SWTS" != type
      )
    {
      std::cout << "Not a shader. File is type: " << type << std::endl;
      return NULL;
    }

  osg::ref_ptr< osg::StateSet > stateSet( new osg::StateSet );
  osg::ref_ptr< osg::Material > mat( new osg::Material );
  mat->setName( shaderFilename );
  stateSet->setAttributeAndModes( mat.get() );
  
  if( NULL != shaderFile.get() )
    {
      float ar, ag, ab, aa;
      float dr, dg, db, da;
      float sr, sg, sb, sa;
      float er, eg, eb, ea;
      float shiny;

      std::string normalTextureName;

      ml::swts animatedShader;
      ml::sht shader;
      ml::cshd cshader;

      //stateSet->setMode(GL_BLEND,osg::StateAttribute::ON);

      if( animatedShader.isRightType( *shaderFile ) )
        {
          animatedShader.readSWTS( *shaderFile );

          std::string texName;
          std::string texTag;
#if 0
          delete shaderFile;
          std::string newShaderName = animatedShader.getShaderFilename();
          loadShader( newShaderName, newShaderName );

          csRef<iTextureWrapper> texWrapper;
          unsigned int numTextures = animatedShader.getNumTextures();
          for( unsigned int i = 0; i < numTextures; ++i )
            {
              animatedShader.getTextureInfo( i, texName, texTag );

              texWrapper = loadOrGetTexture( texName );
            }

          mat =
            engine->GetMaterialList()->FindByName( newShaderName.c_str() );
#endif

          animatedShader.getTextureInfo( 0, texName, texTag );

          std::string diffuseTextureName = texName;
        }
      else if( shader.isRightType( *shaderFile ) )
        {
          shader.readSHT( *shaderFile );
          shader.getAmbient( aa, ab, ag, ar );
	  mat->setAmbient( osg::Material::FRONT, osg::Vec4( ar, ag, ab, aa ) );
          shader.getDiffuse( da, db, dg, dr );
	  mat->setDiffuse( osg::Material::FRONT, osg::Vec4( dr, dg, db, da ) );
          shader.getSpecular( sa, sb, sg, sr );
	  mat->setSpecular( osg::Material::FRONT, osg::Vec4( sr, sg, sb, sa ) );
          shader.getEmissive( ea, eb, eg, er );
	  mat->setEmission( osg::Material::FRONT, osg::Vec4( er, eg, eb, ea ) );
          shader.getShininess( shiny );
	  mat->setShininess( osg::Material::FRONT, shiny );

          std::string diffuseTextureName = shader.getMainTextureName();
	  unsigned int diffuseTextureUnit = shader.getMainTextureUnit();

	  osg::ref_ptr< osg::Texture2D > diffuseTexture =
	    loadTextureFile( diffuseTextureName );

	  if( NULL != diffuseTexture )
	    {
	      diffuseTexture->setWrap( osg::Texture::WRAP_S,
				       osg::Texture::REPEAT );
	      diffuseTexture->setWrap( osg::Texture::WRAP_T,
				       osg::Texture::REPEAT );
	      stateSet->setTextureAttributeAndModes( diffuseTextureUnit,
						     diffuseTexture.get(),
						     osg::StateAttribute::ON );
	    }

          normalTextureName = shader.getNormalTextureName();
        }
      else if( cshader.isRightType( *shaderFile ) )
        {
          cshader.readCSHD( *shaderFile );
          cshader.getAmbient( aa, ab, ag, ar );
	  mat->setAmbient( osg::Material::FRONT, osg::Vec4( ar, ag, ab, aa ) );
          cshader.getDiffuse( da, db, dg, dr );
	  mat->setDiffuse( osg::Material::FRONT, osg::Vec4( dr, dg, db, da ) );
          cshader.getSpecular( sa, sb, sg, sr );
	  mat->setSpecular( osg::Material::FRONT, osg::Vec4( sr, sg, sb, sa ) );
          cshader.getEmissive( ea, eb, eg, er );
	  mat->setEmission( osg::Material::FRONT, osg::Vec4( er, eg, eb, ea ) );
          cshader.getShininess( shiny );
	  mat->setShininess( osg::Material::FRONT, shiny );

          std::string diffuseTextureName = cshader.getMainTextureName();
	  unsigned int diffuseTextureUnit = shader.getMainTextureUnit();
          normalTextureName = shader.getNormalTextureName();

	  osg::ref_ptr< osg::Texture2D > diffuseTexture =
	    loadTextureFile( diffuseTextureName );

	  std::cout << __FILE__ << ": " << __LINE__ << ": " << diffuseTextureUnit<< std::endl;

	  if( NULL != diffuseTexture )
	    {
	      diffuseTexture->setWrap( osg::Texture::WRAP_S,
				       osg::Texture::REPEAT );
	      diffuseTexture->setWrap( osg::Texture::WRAP_T,
				       osg::Texture::REPEAT );
	      stateSet->setTextureAttributeAndModes( diffuseTextureUnit,
						     diffuseTexture.get(),
						     osg::StateAttribute::ON );
	    }
	  std::cout << __FILE__ << ": " << __LINE__ << std::endl;

        }
      else
        {
          std::cout << "File not .SHT or .CSHD" << std::endl;
        }

      std::cout << "Creating material: " << shaderFilename
                << std::endl;


      if( NULL == mat )
        {
          std::cout << "Material creation failed" << std::endl;
        }
      else
        {
          std::cout << "Material created: "
                    << mat->getName()
                    << std::endl;
        }

#if 0
      if( !commonShader->getNormalTextureName().empty() )
        {
          std::cout << "Creating normal shader variable" << std::endl;
          csRef<csShaderVariable> normalSV =
            mat->GetMaterial()->GetVariableAdd(strings->Request("tex normal"));

          std::cout << "Loading/Getting normal texture: "
                    << normalTextureName
                    << std::endl;
          csRef<iTextureWrapper> texNorm =
            loadOrGetTexture( normalTextureName );
          texNorm->SetTextureClass ("normalmap");
          normalSV->SetValue( texNorm );
        }

#endif
     
    } //if NULL != shaderFile

  stateMap[ shaderFilename ] = stateSet;

  return stateSet;
}

osg::ref_ptr< osg::Node >
swgRepository::loadPRTO( std::auto_ptr<std::istream> prtoFile )
{
  // Read from stream into prto record
  ml::prto swgPRTO;
  swgPRTO.readPRTO( *prtoFile );
  
  osg::ref_ptr<osg::Group> prtoMesh( new osg::Group );

  unsigned int numCells = swgPRTO.getNumCells();
  for( unsigned int i = 0; i < numCells; ++i )
    {
      ml::cell &currentCell = swgPRTO.getCell( i );

      osg::ref_ptr<osg::Node> cellModel
	= loadFile( currentCell.getModelFilename() );

      if( NULL != cellModel )
	{
	  prtoMesh->addChild( cellModel.get() );
	}
    }

  return prtoMesh;
}

osg::ref_ptr< osg::Node >
swgRepository::loadMSH( std::auto_ptr<std::istream> meshFile )
{
  // Read from stream into msh record
  ml::msh swgMesh;
  unsigned int size = swgMesh.readMSH( *meshFile );
  if( 0 == size )
    {
      return NULL;
    }

  // Pointers to vertex and index data.
  ml::mshVertexData *vData;  
  ml::mshVertexIndex *iData;

  // Create new geode to store geometry.
  osg::ref_ptr< osg::Geode > geode( new osg::Geode() );

  std::string shaderFilename;
  float x, y, z;
  unsigned char argb[4];
  unsigned int numTexCoordPairs;
  float texCoord[12];

  // Loop through all the sets of vertex indices.
  for(unsigned int indexTable = 0; indexTable < swgMesh.getNumIndexTables();
      ++indexTable )
    {
      osg::Vec3Array* vertices = new osg::Vec3Array;
      osg::Vec3Array* normals = new osg::Vec3Array;
      osg::Vec4Array* colors = new osg::Vec4Array;

      std::vector< osg::ref_ptr< osg::Vec2Array > > texCoordVec;
      for( unsigned int i = 0; i < ml::MAX_TEXTURES; ++i )
	{
	  texCoordVec.push_back( new osg::Vec2Array );
	}
      
      swgMesh.getIndex( indexTable, &vData, &iData, shaderFilename );
      unsigned int numVertices = vData->getNumVertices();
      std::cout << "Adding " << numVertices << " vertices" << std::endl;

      // Build list of vertices used with current indices.
      for( unsigned int i = 0; i < numVertices; ++i )
	{
	  (vData->getVertex( i ))->getPosition( x, y, z );
	  vertices->push_back( osg::Vec3( z, y, x ) );
	  
	  //std::cout << x << ", " << y << ", " << z << std::endl;
	  
	  (vData->getVertex( i ))->getNormal( x, y, z );
	  normals->push_back( osg::Vec3( z, y, x ) );
	  
	  // Hard code color for now
	  (vData->getVertex( i ))->getColor( argb );
	  //colors->push_back( osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
	  colors->push_back( osg::Vec4( argb[1]/255.0,
					argb[2]/255.0,
					argb[3]/255.0,
					argb[0]/255.0 )
			     );
	  
	  
	  (vData->getVertex( i ))->getTexCoords( numTexCoordPairs,
						 texCoord );
	  
	  for( unsigned int j = 0; j < numTexCoordPairs; ++j )
	    {
	      texCoordVec[j]->push_back(
					osg::Vec2( texCoord[j*2],
						   texCoord[(j*2)+1] )
					);
	    }
	}
      
      // Create new geometry node list of vertex attributes.
      osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
      
      geometry->setVertexArray( vertices );
      
      geometry->setColorArray( colors );
      geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
      
      geometry->setNormalArray( normals );
      geometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );      

      for( unsigned int j = 0; j < ml::MAX_TEXTURES; ++ j )
	{
	  if( !(texCoordVec[j]->empty()) )
	    {
	      geometry->setTexCoordArray( j, texCoordVec[j].get() );
	    }
	}

      unsigned int numIndices = iData->getNumIndices();
      std::cout << "Num indices: " << numIndices << std::endl;
      
      // Create new primitive set to hold this list.
      osg::DrawElementsUShort* drawElements =
	new osg::DrawElementsUShort( osg::PrimitiveSet::TRIANGLES );
      drawElements->reserve( numIndices );
      
      // Populate primitive set with indices.
      for( unsigned int i = 0; i < numIndices; ++i )
	{
	  if( iData->getIndex( i ) >= numVertices )
	    {
	      std::cout << "Indexing outside vertex list: "
			<< iData->getIndex( i )
			<< std::endl;
	    }
	  drawElements->push_back( iData->getIndex( i ) );
	}

      // Add primitive set to this geometry node.
      geometry->addPrimitiveSet( drawElements );
      
      // Load shader and attach to this geometry node.
      std::string shaderFilename = swgMesh.getShader( iData->getShaderIndex() );
      geometry->setStateSet( loadShader( shaderFilename ) );

      osg::VertexBufferObject *vbo = new osg::VertexBufferObject;
      vertices->setVertexBufferObject( vbo );

      osg::ElementBufferObject* ebo = new osg::ElementBufferObject;
      drawElements->setElementBufferObject( ebo );

      geometry->setUseVertexBufferObjects( ( NULL != vbo ) );

      geode->addDrawable( geometry.get() );
    }

  return geode;
}

osg::ref_ptr< osg::Node >
swgRepository::loadSKMG( std::auto_ptr<std::istream> meshFile )
{
  // Read from stream into skmg record
  ml::skmg swgSKMG;
  unsigned int size = swgSKMG.readSKMG( *meshFile );
  if( 0 == size )
    {
      return NULL;
    }

  // Create new geode to store geometry.
  osg::ref_ptr< osg::Geode > geode( new osg::Geode() );


  unsigned int numPsdt = swgSKMG.getNumPsdt();

  for( unsigned int j = 0; j < numPsdt; ++j )
    {
      osg::Vec3Array* vertices = new osg::Vec3Array;
      osg::Vec3Array* normals = new osg::Vec3Array;
      osg::Vec4Array* colors = new osg::Vec4Array;
      osg::Vec2Array* texCoords = new osg::Vec2Array;
      
      const ml::skmg::psdt &newPsdt = swgSKMG.getPsdt( j );

      std::cout << "Adding " << newPsdt.getNumVertex() << " vertices" << std::endl;
      
      // Build list of vertices used with current indices.
      float x, y, z;
      for( unsigned int i = 0; i < newPsdt.getNumVertex(); ++i )
	{
	  newPsdt.getVertex( i, x, y, z );
	  vertices->push_back( osg::Vec3( z, y, x ) );
	  
	  std::cout << "xyz: " << x << ", " << y << ", " << z << std::endl;
	  
	  newPsdt.getNormal( i, x, y, z );
	  normals->push_back( osg::Vec3( z, y, x ) );
	  
	  // Hard code color for now
	  colors->push_back( osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
	  
	  newPsdt.getTexCoord( i, x, y );
	  texCoords->push_back( osg::Vec2( x, y ) );
	  std::cout << "uv: " << x << ", " << y << std::endl;
	}
      
      // Create new geometry node list of vertex attributes.
      osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
      
      geometry->setVertexArray( vertices );
      
      geometry->setColorArray( colors );
      geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
      
      geometry->setNormalArray( normals );
      geometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );      
      
      geometry->setTexCoordArray( 0, texCoords );
      
      std::cout << "Num groups: " << swgSKMG.getNumGroups() << std::endl;
      osg::ElementBufferObject* ebo = new osg::ElementBufferObject;
      for( unsigned short int i = 0; i <= swgSKMG.getNumGroups(); ++i )
	{
	  // Create new primitive set to hold this list.
	  osg::DrawElementsUShort* drawElements =
	    new osg::DrawElementsUShort( osg::PrimitiveSet::TRIANGLES );
      
	  const std::vector<unsigned int> &oitl = newPsdt.getOTriangles( i-1 );
	  std::cout << "Group " << (i-1) << ": Num triangles: "
		    << (oitl.size()/3) << std::endl;
	  // Populate primitive set with indices.
	  for( unsigned int j = 0; j < oitl.size(); ++j )
	    {
	      drawElements->push_back( oitl[j] );
	    }
	  
	  // Add primitive set to this geometry node.
	  drawElements->setElementBufferObject( ebo );
	  geometry->addPrimitiveSet( drawElements );
	}
      
      {
	// Create new primitive set to hold this list.
	osg::DrawElementsUShort* drawElements =
	  new osg::DrawElementsUShort( osg::PrimitiveSet::TRIANGLES );
	
	const std::vector<unsigned int> &itl = newPsdt.getTriangles();
	// Populate primitive set with indices.
	for( unsigned int j = 0; j < itl.size(); ++j )
	  {
	    drawElements->push_back( itl[j] );
	  }
	
	// Add primitive set to this geometry node.
	drawElements->setElementBufferObject( ebo );
	geometry->addPrimitiveSet( drawElements );
      }  
      
      // Load shader and attach to this geometry node.
      std::string shaderFilename = newPsdt.getShader();
      geometry->setStateSet( loadShader( shaderFilename ) );
      
      osg::VertexBufferObject *vbo = new osg::VertexBufferObject;
      vertices->setVertexBufferObject( vbo );
      
      geometry->setUseVertexBufferObjects( ( NULL != vbo ) );
      
      geode->addDrawable( geometry.get() );
    }   
  return geode;
}

osg::ref_ptr< osg::Node >
swgRepository::loadLOD( std::auto_ptr<std::istream> lodFile )
{
  // Read from stream into lod record
  ml::lod swgLOD;
  swgLOD.readLOD( *lodFile );
  
  osg::ref_ptr<osg::LOD> lodMesh = new osg::LOD;
  
  unsigned int numLODs = swgLOD.getNumLODs();
  std::cout << "Num LODs: " << numLODs << std::endl;
  std::string childFilename;
  float near, far;
  for( unsigned int i = 0; i < numLODs; ++i )
    {
      swgLOD.getChild( i, childFilename, near, far );

      osg::ref_ptr<osg::Node> childMesh = loadFile( childFilename );
      
      if( NULL != childMesh )
        {
          lodMesh->addChild( childMesh, near, far );
        }
    }

  return lodMesh;
}

osg::ref_ptr< osg::Node >
swgRepository::loadCMP( std::auto_ptr<std::istream> cmpFile )
{
  // Read from stream into cmp record
  ml::cmp swgCMP;
  swgCMP.readCMP( *cmpFile );
  
  osg::ref_ptr<osg::Group> cmpMesh( new osg::Group );

  unsigned int numParts = swgCMP.getNumParts();
  std::cout << "Num parts: " << numParts << std::endl;
  std::string partFilename;
  for( unsigned int i = 0; i < numParts; ++i )
    {
      ml::vector3 partPosition;
      ml::matrix3 partScaleRotate;
      swgCMP.getPart( i, partFilename, partPosition, partScaleRotate );

      osg::Node *partMesh = loadFile( partFilename );
      
      if( NULL != partMesh )
        {
	  osg::ref_ptr<osg::MatrixTransform>
	    partTrans = new osg::MatrixTransform;
	  partTrans->addChild( partMesh );
	  osg::Matrix matrix( partScaleRotate.get( 0 ),
			      partScaleRotate.get( 1 ),
			      partScaleRotate.get( 2 ),
			      partPosition.getX(),
			      
			      partScaleRotate.get( 3 ),
			      partScaleRotate.get( 4 ),
			      partScaleRotate.get( 5 ),
			      partPosition.getY(),
			      
			      partScaleRotate.get( 6 ),
			      partScaleRotate.get( 7 ),
			      partScaleRotate.get( 8 ),
			      partPosition.getZ(),
			      
			      0.0, 0.0, 0.0, 1.0
			      );
	  partTrans->setMatrix( matrix );
          cmpMesh->addChild( partTrans );
        }
    }

  return cmpMesh;
}

osg::ref_ptr< osg::Node >
swgRepository::loadAPT( std::auto_ptr<std::istream> aptFile )
{
  // Read from stream into apt record
  ml::apt swgAPT;
  swgAPT.readAPT( *aptFile );
  
  osg::ref_ptr<osg::Group> aptMesh( new osg::Group );

  std::string childFilename;
  childFilename = swgAPT.getChildFilename();

  osg::ref_ptr<osg::Node> childMesh = loadFile( childFilename );

  if( NULL != childMesh )
    {
      aptMesh->addChild( childMesh.get() );
    }

  return aptMesh;
}

osg::ref_ptr<osg::Node>
swgRepository::loadSTAT( std::auto_ptr<std::istream> statFile )
{
  // Read from stream into stat record
  ml::stat swgSTAT;
  swgSTAT.readSTAT( *statFile );
  
  osg::ref_ptr<osg::Group> statMesh( new osg::Group );

  std::string appearanceFilename( swgSTAT.getAppearanceFilename() );

  osg::ref_ptr<osg::Node> appearanceMesh( loadFile( appearanceFilename ) );
  
  if( NULL != appearanceMesh )
    {
      statMesh->addChild( appearanceMesh.get() );
    }

  return statMesh;
}

osg::ref_ptr< osg::Node >
swgRepository::loadSTOT( std::auto_ptr<std::istream> stotFile )
{
  // Read from stream into stot record
  ml::stot swgSTOT;
  swgSTOT.readSTOT( *stotFile );
  
  osg::ref_ptr<osg::Group> stotMesh( new osg::Group );

  std::string appearanceFilename( swgSTOT.getAppearanceFilename() );

  osg::ref_ptr<osg::Node> appearanceMesh( loadFile( appearanceFilename ) );

  if( NULL != appearanceMesh )
    {
      stotMesh->addChild( appearanceMesh.get() );
    }

  return stotMesh;
}

osg::ref_ptr< osg::Node >
swgRepository::loadSBOT( std::auto_ptr<std::istream> sbotFile )
{
  // Read from stream into sbot record
  ml::sbot swgSBOT;
  swgSBOT.readSBOT( *sbotFile );
  
  osg::ref_ptr<osg::Group> sbotMesh( new osg::Group );

  std::string filename( swgSBOT.getAppearanceFilename() );
  osg::ref_ptr<osg::Node> appearanceMesh( loadFile( filename ) );
  if( NULL != appearanceMesh )
    {
      sbotMesh->addChild( appearanceMesh );
    }

  filename = swgSBOT.getPortalLayoutFilename();
  osg::ref_ptr<osg::Node> portalLayoutMesh( loadFile( filename ) );
  if( NULL != portalLayoutMesh )
    {
      sbotMesh->addChild( portalLayoutMesh );
    }

  filename = swgSBOT.getInteriorLayoutFilename();
  osg::ref_ptr<osg::Node>
    interiorLayoutMesh( loadFile( filename ) );
  if( NULL != interiorLayoutMesh )
    {
      sbotMesh->addChild( interiorLayoutMesh );
    }

  return sbotMesh;
}

osg::ref_ptr<osg::Node> 
swgRepository::loadINLY( std::auto_ptr<std::istream> inlyFile )
{
  // Read from stream into inly record
  ml::ilf swgINLY;
  swgINLY.readILF( *inlyFile );
  
  osg::ref_ptr<osg::Group> inlyMesh = new osg::Group;

  unsigned int numNodes = swgINLY.getNumNodes();
  std::cout << "Number of object nodes: " << numNodes << std::endl;

  for( unsigned int i = 0; i < numNodes; ++i )
    {
      std::string nodeFilename;
      std::string zoneName;
      ml::matrix3 nodeRot;
      ml::vector3 nodeTrans;
      swgINLY.getNode( i, nodeFilename, zoneName,
		       nodeRot, nodeTrans );

      osg::ref_ptr<osg::Node> node = loadFile( nodeFilename );
      
      if( NULL != node )
	{
	  osg::ref_ptr<osg::MatrixTransform> transform
	    = new osg::MatrixTransform;
#if 0
	  osg::Matrix rotMat( nodeRot.get(0),
			      nodeRot.get(1),
			      nodeRot.get(2),
			      0.0,
			      nodeRot.get(3),
			      nodeRot.get(4),
			      nodeRot.get(5),
			      0.0,
			      nodeRot.get(6),
			      nodeRot.get(7),
			      nodeRot.get(8),
				    0.0,
			      0.0, 0.0, 0.0, 1.0
			      );
#else
	  osg::Matrix rotMat( nodeRot.get(0),nodeRot.get(3),-nodeRot.get(6), 0.0,
			      nodeRot.get(1),nodeRot.get(4),-nodeRot.get(7), 0.0,
			      -nodeRot.get(2),-nodeRot.get(5),nodeRot.get(8), 0.0,
			      0.0, 0.0, 0.0, 1.0
			      );
#endif
	  osg::Matrix transMat( osg::Matrix::translate( nodeTrans.getZ(),
							nodeTrans.getY(),
							nodeTrans.getX())
				);
	  transform->setMatrix( rotMat * transMat );
	  transform->addChild( loadFile( nodeFilename ) );

	  inlyMesh->addChild( transform.get() );
	}
    }

  return inlyMesh;
}

osg::ref_ptr<osg::Node> 
swgRepository::loadWSNP( std::auto_ptr<std::istream> wsnpFile )
{
  // Read from stream into wsnp record
  ml::ws swgWSNP;
  swgWSNP.readWS( *wsnpFile );
  
  osg::ref_ptr<osg::MatrixTransform> wsnpMesh = new osg::MatrixTransform;
  wsnpMesh->setMatrix( osg::Matrix::rotate( -osg::PI, 1, 0, 0 ) );

  unsigned int numObjects = swgWSNP.getNumObjectNodes();
  std::cout << "Number of object nodes: " << numObjects << std::endl;

  std::map< unsigned int, osg::ref_ptr< osg::MatrixTransform > >
    wsNodeMap;

  for( unsigned int i = 0; i < numObjects; ++i )
  //for( unsigned int i = 0; i < 1000; ++i )
    {
      ml::wsNode &node = swgWSNP.getObjectNode( i );
      std::string objectFilename( node.getObjectFilename() );
      
      std::cout << "Loading object node: " << objectFilename << std::endl;
      
      osg::ref_ptr<osg::Node> objectMesh = loadFile( objectFilename );
      
      osg::Quat nodeQuat( node.getQuatX(),
			  node.getQuatY(),
			  node.getQuatZ(),
			  node.getQuatW() );
      osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
      osg::Matrix rotMat( osg::Matrix::rotate( nodeQuat ) );

      osg::Matrix trotMat(
		       rotMat(0,0), rotMat(1,0), -rotMat(2,0), rotMat(3,0),
		       rotMat(0,1), rotMat(1,1), -rotMat(2,1), rotMat(3,1),
		       -rotMat(0,2), -rotMat(1,2), rotMat(2,2), -rotMat(3,2),
		       rotMat(0,3), rotMat(1,3), -rotMat(2,3), rotMat(3,3)
		       );
      
      osg::Matrix matrix( 
			 trotMat
			 * osg::Matrix::rotate( -osg::PI_2, 0, 1, 0 )
			 * osg::Matrix::translate( node.getX(),
						   node.getY(),
						   node.getZ())
			  );
      transform->setMatrix( matrix );
      transform->addChild( objectMesh );
      
      wsNodeMap[ node.getID() ] = transform;
      
      if( 0 == node.getParentID() )
	{
	  wsnpMesh->addChild( transform );
	}
      else
	{
	  wsNodeMap[ node.getParentID() ]->addChild( transform );
	}
    }

  return wsnpMesh;
}

osg::Geode* createAxis()
{
    osg::Geode* geode (new osg::Geode());
    osg::Geometry* geometry (new osg::Geometry());

    osg::Vec3Array* vertices (new osg::Vec3Array());
    vertices->push_back (osg::Vec3 ( 0.0, 0.0, 0.0));
    vertices->push_back (osg::Vec3 ( 1.0, 0.0, 0.0));
    vertices->push_back (osg::Vec3 ( 0.0, 0.0, 0.0));
    vertices->push_back (osg::Vec3 ( 0.0, 1.0, 0.0));
    vertices->push_back (osg::Vec3 ( 0.0, 0.0, 0.0));
    vertices->push_back (osg::Vec3 ( 0.0, 0.0, 1.0));
    geometry->setVertexArray (vertices);

    osg::Vec4Array* colors (new osg::Vec4Array());
    colors->push_back (osg::Vec4 (1.0f, 0.0f, 0.0f, 1.0f));
    colors->push_back (osg::Vec4 (1.0f, 0.0f, 0.0f, 1.0f));
    colors->push_back (osg::Vec4 (0.0f, 1.0f, 0.0f, 1.0f));
    colors->push_back (osg::Vec4 (0.0f, 1.0f, 0.0f, 1.0f));
    colors->push_back (osg::Vec4 (0.0f, 0.0f, 1.0f, 1.0f));
    colors->push_back (osg::Vec4 (0.0f, 0.0f, 1.0f, 1.0f));
    geometry->setColorArray (colors);

    geometry->setColorBinding (osg::Geometry::BIND_PER_VERTEX);
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,6));
    geode->getOrCreateStateSet()->setMode(GL_LIGHTING, false);

    geode->addDrawable( geometry );
    return geode;
}

osg::Geode* createLine( const osg::Vec3 &xyz )
{
    osg::Geode* geode( new osg::Geode() );
    osg::Geometry* geometry( new osg::Geometry() );

    osg::Vec3Array* vertices( new osg::Vec3Array() );
    vertices->push_back( osg::Vec3( 0.0, 0.0, 0.0) );
    vertices->push_back( xyz );
    geometry->setVertexArray (vertices);

    osg::Vec4Array* colors( new osg::Vec4Array() );
    colors->push_back( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
    colors->push_back( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
    geometry->setColorArray( colors );

    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
    geometry->addPrimitiveSet(
			      new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2)
			      );
    geode->getOrCreateStateSet()->setMode(GL_LIGHTING, false);

    geode->addDrawable( geometry );
    return geode;
}

osg::ref_ptr< osgAnimation::Skeleton >
swgRepository::loadSKTM( std::auto_ptr<std::istream> sktmFile )
{
  ml::sktm swgSKTM;
  swgSKTM.readSKTM( *sktmFile );

  osg::ref_ptr<osgAnimation::Skeleton> skeleton = new osgAnimation::Skeleton;
  
  // Loop through and create all bones...
  std::vector< osg::ref_ptr<osgAnimation::Bone> > boneList;
  for( unsigned int i = 0; i < swgSKTM.getNumBones(); ++i )
    {
      osg::ref_ptr<osgAnimation::Bone> newBone = new osgAnimation::Bone;

      osg::Quat postQuat( swgSKTM.getBonePostQuatX( i ),
			  swgSKTM.getBonePostQuatY( i ),
			  swgSKTM.getBonePostQuatZ( i ),
			  swgSKTM.getBonePostQuatW( i ) );
      
      osg::Quat preQuat( swgSKTM.getBonePreQuatX( i ),
			 swgSKTM.getBonePreQuatY( i ),
			 swgSKTM.getBonePreQuatZ( i ),
			 swgSKTM.getBonePreQuatW( i ) );

      newBone->setRotation( preQuat * postQuat );

      newBone->setTranslation(osg::Vec3(
					swgSKTM.getBoneXOffset( i ),
					swgSKTM.getBoneYOffset( i ),
					swgSKTM.getBoneZOffset( i ))
			      );

      newBone->setName( swgSKTM.getBoneName( i ) );
      newBone->addChild( createAxis() );

      osgText::Text* text = new osgText::Text;
      text->setText( swgSKTM.getBoneName( i ) );
      text->setCharacterSize( 60.0 );
      text->setAxisAlignment(osgText::Text::SCREEN);
      text->setCharacterSizeMode(osgText::Text::SCREEN_COORDS);
 
      osg::Geode* geode  = new osg::Geode;
      geode->addDrawable( text );
      newBone->addChild( geode );

      boneList.push_back( newBone );
    }

  // Loop through and assign all bones to parents...
  for( unsigned int i = 0; i < swgSKTM.getNumBones(); ++i )
    {
      int parent = swgSKTM.getBoneParent( i );

      if( parent >= 0 )
	{
	  boneList[parent]->addChild( boneList[i].get() );
	  boneList[parent]->addChild( createLine(boneList[i]->getTranslation()) );
	}
      else
	{
	  skeleton->addChild(  boneList[i].get() );
	}
    }
  
  return skeleton;
}

osg::ref_ptr< osg::Node >
swgRepository::loadMLOD( std::auto_ptr<std::istream> mlodFile )
{
  // Read from stream into mlod record
  ml::mlod swgMLOD;
  swgMLOD.readMLOD( *mlodFile );
  
  osg::ref_ptr<osg::LOD> mlodMesh = new osg::LOD;
  
  unsigned int numMLODs = swgMLOD.getNumMesh();
  std::cout << "Num MLODs: " << numMLODs << std::endl;
  std::string childFilename;
  //float near, far;
  for( unsigned int i = 0; i < numMLODs; ++i )
    {
      osg::ref_ptr<osg::Node> childMesh =
	loadFile( swgMLOD.getMeshFilename( i ) );
      
      if( NULL != childMesh )
        {
          mlodMesh->addChild( childMesh, i*100.0, (i+1) * 100.0 );
        }
    }

  return mlodMesh;
}

#if 0
osg::Geode* createPolygon( const ml::trn::bpol &polygon, float alt )
{
    osg::Geode* geode( new osg::Geode() );
    osg::Geometry* geometry( new osg::Geometry() );

    osg::Vec3Array* vertices( new osg::Vec3Array() );
    for( unsigned int i = 0; i < polygon.x.size(); ++i )
      {
	vertices->push_back( osg::Vec3( polygon.x[i],
					polygon.y[i],
					alt*10.0 )
			     );
      }

    geometry->setVertexArray( vertices );

    osg::Vec4Array* colors( new osg::Vec4Array() );
    colors->push_back( osg::Vec4( 0.0f, 0.0f, 1.0f, 0.3f ) );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    geometry->addPrimitiveSet(
			      new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,
						  0, vertices->size())
			      );
    geode->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
    geode->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);

    geode->addDrawable( geometry );
    return geode;
}

osg::Geode* createPolyline( const ml::trn::bpln &polyline, float alt )
{
    osg::Geode* geode( new osg::Geode() );
    osg::Geometry* geometry( new osg::Geometry() );

    osg::Vec3Array* vertices( new osg::Vec3Array() );
    for( unsigned int i = 0; i < polyline.x.size(); ++i )
      {
	vertices->push_back( osg::Vec3( polyline.x[i],
					polyline.y[i],
					alt*10.0 )
			     );
      }

    geometry->setVertexArray( vertices );

    osg::Vec4Array* colors( new osg::Vec4Array() );
    colors->push_back( osg::Vec4( 1.0f, 0.0f, 0.0f, 0.3f ) );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    geometry->addPrimitiveSet(
			      new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP,
						  0, vertices->size())
			      );
    geode->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
    geode->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);

    geode->addDrawable( geometry );
    return geode;
}

osg::Geode* createRectangle( const ml::trn::brec &rec, float alt )
{
    osg::Geode* geode( new osg::Geode() );
    osg::Geometry* geometry( new osg::Geometry() );

    osg::Vec3Array* vertices( new osg::Vec3Array() );
    vertices->push_back( osg::Vec3( rec.x1, rec.y1, alt*10.0 ) );
    vertices->push_back( osg::Vec3( rec.x2, rec.y1, alt*10.0 ) );
    vertices->push_back( osg::Vec3( rec.x2, rec.y2, alt*10.0 ) );
    vertices->push_back( osg::Vec3( rec.x1, rec.y2, alt*10.0 ) );

    geometry->setVertexArray( vertices );

    osg::Vec4Array* colors( new osg::Vec4Array() );
    colors->push_back( osg::Vec4( 0.0f, 1.0f, 0.0f, 0.3f ) );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    geometry->addPrimitiveSet(
			      new osg::DrawArrays(osg::PrimitiveSet::POLYGON,
						  0, vertices->size())
			      );
    geode->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
    geode->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);

    geode->addDrawable( geometry );
    return geode;
}

osg::Geode* createCircle( const ml::trn::bcir &cir, float alt )
{
    osg::Geode* geode( new osg::Geode() );

    geode->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
    geode->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);

    geode->addDrawable(
		       new osg::ShapeDrawable(
					      new osg::Cylinder(
								osg::Vec3(cir.x,
									  cir.y,
									  0.0f),
								cir.radius,
								0.1 )
					      )
		       );
    return geode;
}

osg::ref_ptr< osg::Node >
swgRepository::loadTRN( std::auto_ptr<std::istream> trnFile )
{
  // Read from stream into trn record
  ml::trn swgTRN;
  swgTRN.readTRN( *trnFile );
  
  osg::ref_ptr<osg::Group> trnMesh( new osg::Group );

  const std::vector<ml::trn::bpol> bpolList = swgTRN.getBPOL();
  for( unsigned int i = 0; i < bpolList.size(); ++i )
    {
      trnMesh->addChild( createPolygon( bpolList[i], i ) );
    }

  const std::vector<ml::trn::brec> brecList = swgTRN.getBREC();
  for( unsigned int i = 0; i < brecList.size(); ++i )
    {
      trnMesh->addChild( createRectangle( brecList[i], -(float(i)) ) );
    }

  const std::vector<ml::trn::bcir> bcirList = swgTRN.getBCIR();
  for( unsigned int i = 0; i < bcirList.size(); ++i )
    {
      trnMesh->addChild( createCircle( bcirList[i], i ) );
    }

  const std::vector<ml::trn::bpln> bplnList = swgTRN.getBPLN();
  for( unsigned int i = 0; i < bplnList.size(); ++i )
    {
      trnMesh->addChild( createPolyline( bplnList[i], i ) );
    }

  return trnMesh;
}
#endif

osg::Geode* createWater( const float &terrainHalfSize, const float &height )
{
    osg::Geode* geode( new osg::Geode() );
    osg::Geometry* geometry( new osg::Geometry() );

    osg::Vec3Array* vertices( new osg::Vec3Array() );
    vertices->push_back(osg::Vec3(-terrainHalfSize, -terrainHalfSize, height));
    vertices->push_back(osg::Vec3(-terrainHalfSize, terrainHalfSize, height));
    vertices->push_back(osg::Vec3(terrainHalfSize, -terrainHalfSize, height));
    vertices->push_back(osg::Vec3(terrainHalfSize, terrainHalfSize, height));

    geometry->setVertexArray( vertices );

    osg::Vec4Array* colors( new osg::Vec4Array() );
    colors->push_back( osg::Vec4( 0.0f, 0.0f, 1.0f, 0.3f ) );
    geometry->setColorArray( colors );
    geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    geometry->addPrimitiveSet(
			      new osg::DrawArrays(
						  osg::PrimitiveSet::TRIANGLE_STRIP,
						  0, vertices->size())
			      );
    geode->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
    geode->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::ON);

    geode->addDrawable( geometry );
    return geode;
}

osg::ref_ptr< osg::Node >
swgRepository::loadTRN( std::auto_ptr<std::istream> trnFile )
{
  // Read from stream into trn record
  ml::trn swgTRN;
  swgTRN.readTRN( *trnFile );

  float terrainSize = swgTRN.getTerrainSize();
  float waterLevel = swgTRN.getWaterTableHeight();
  
  float originX = -terrainSize/2.0;
  float originY = -terrainSize/2.0;

  float spacing = 50.0;

  unsigned int numRows = static_cast<unsigned int>( terrainSize / spacing );
  unsigned int numColumns = numRows;

  std::cout << "Height: " << terrainSize << std::endl;
  std::cout << "Width: " << terrainSize << std::endl;

  std::cout << "Num rows: " << numRows << std::endl;
  std::cout << "Num cols: " << numRows << std::endl;

  // Need to build a heightmap.
  float *data = new float[numRows*numColumns];
  for( unsigned int i = 0; i < (numRows*numColumns); ++i )
    {
      data[i] = waterLevel;
    }
  swgTRN.applyLayers( originX, originY, spacing, spacing,
		      numRows, numColumns, data );
		      
  osg::ref_ptr<osg::Group> trnMesh( new osg::Group );

  osg::HeightField *grid = new osg::HeightField;
  grid->allocate( numColumns, numRows );
  grid->setOrigin( osg::Vec3( originX, originY, 0.0 ) );
  grid->setXInterval( spacing );
  grid->setYInterval( spacing );

  for( unsigned int row = 0; row < numRows; ++row )
    {
      unsigned int offset = row*numColumns;
      for( unsigned int col = 0; col < numColumns; ++col )
	{
	  grid->setHeight( col, row, data[offset+col] );
	}
    }

  delete[] data;

  osg::Geode* geode = new osg::Geode();
  geode->addDrawable( new osg::ShapeDrawable( grid ) );

  osg::MatrixTransform *matTrans = new osg::MatrixTransform();
  matTrans->setMatrix(
                      osg::Matrix::rotate(
                                          osg::DegreesToRadians( -90.0 ),
                                          1.0, 0.0, 0.0 )
                      );
  matTrans->addChild( geode );

  matTrans->addChild( createWater( terrainSize/2.0, waterLevel ) );


  trnMesh->addChild( matTrans );

  return trnMesh;
}


void swgRepository::createArchive( const std::string &basePath )
{
  archive.addFile( basePath+"bottom.tre" );
  archive.addFile( basePath+"data_animation_00.tre" );
  archive.addFile( basePath+"data_music_00.tre" );
  archive.addFile( basePath+"data_other_00.tre" );
  archive.addFile( basePath+"data_sample_00.tre" );
  archive.addFile( basePath+"data_sample_01.tre" );
  archive.addFile( basePath+"data_sample_02.tre" );
  archive.addFile( basePath+"data_sample_03.tre" );
  archive.addFile( basePath+"data_sample_04.tre" );
  archive.addFile( basePath+"data_skeletal_mesh_00.tre" );
  archive.addFile( basePath+"data_skeletal_mesh_01.tre" );
  archive.addFile( basePath+"data_sku1_00.tre" );
  archive.addFile( basePath+"data_sku1_01.tre" );
  archive.addFile( basePath+"data_sku1_02.tre" );
  archive.addFile( basePath+"data_sku1_03.tre" );
  archive.addFile( basePath+"data_sku1_04.tre" );
  archive.addFile( basePath+"data_sku1_05.tre" );
  archive.addFile( basePath+"data_sku1_06.tre" );
  archive.addFile( basePath+"data_sku1_07.tre" );
  archive.addFile( basePath+"data_static_mesh_00.tre" );
  archive.addFile( basePath+"data_static_mesh_01.tre" );
  archive.addFile( basePath+"data_texture_00.tre" );
  archive.addFile( basePath+"data_texture_01.tre" );
  archive.addFile( basePath+"data_texture_02.tre" );
  archive.addFile( basePath+"data_texture_03.tre" );
  archive.addFile( basePath+"data_texture_04.tre" );
  archive.addFile( basePath+"data_texture_05.tre" );
  archive.addFile( basePath+"data_texture_06.tre" );
  archive.addFile( basePath+"data_texture_07.tre" );
  archive.addFile( basePath+"default_patch.tre" );
  archive.addFile( basePath+"patch_00.tre" );
  archive.addFile( basePath+"patch_01.tre" );
  archive.addFile( basePath+"patch_02.tre" );
  archive.addFile( basePath+"patch_03.tre" );
  archive.addFile( basePath+"patch_04.tre" );
  archive.addFile( basePath+"patch_05.tre" );
  archive.addFile( basePath+"patch_06.tre" );
  archive.addFile( basePath+"patch_07.tre" );
  archive.addFile( basePath+"patch_08.tre" );
  archive.addFile( basePath+"patch_09.tre" );
  archive.addFile( basePath+"patch_10.tre" );
  archive.addFile( basePath+"patch_11_00.tre" );
  archive.addFile( basePath+"patch_11_01.tre" );
  archive.addFile( basePath+"patch_11_02.tre" );
  archive.addFile( basePath+"patch_11_03.tre" );
  archive.addFile( basePath+"patch_12_00.tre" );
  archive.addFile( basePath+"patch_13_00.tre" );
#if 1
  archive.addFile( basePath+"hotfix_24_client_00.tre" );
  archive.addFile( basePath+"hotfix_24_shared_00.tre" );
  archive.addFile( basePath+"hotfix_26_client_00.tre" );
  archive.addFile( basePath+"hotfix_26_shared_00.tre" );
  archive.addFile( basePath+"hotfix_28_client_00.tre" );
  archive.addFile( basePath+"hotfix_28_shared_00.tre" );
  archive.addFile( basePath+"hotfix_29_client_00.tre" );
  archive.addFile( basePath+"hotfix_29_shared_00.tre" );
  archive.addFile( basePath+"hotfix_sku1_19_client_00.tre" );
  archive.addFile( basePath+"hotfix_sku1_20_client_00.tre" );
  archive.addFile( basePath+"hotfix_sku1_21_client_00.tre" );
  archive.addFile( basePath+"hotfix_sku1_23_client_00.tre" );
  archive.addFile( basePath+"hotfix_sku1_28_client_00.tre" );
#endif
}

