// Reader/Writer driver for .mgn files

#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4250 )
#endif

#include <osgDB/Registry>

#ifdef WIN32
#pragma warning( pop )
#endif

#include "meshLib/skmg.hpp"

namespace osgswg {

    class ReaderWriterMGN : public osgDB::ReaderWriter
    {
    public:
        ReaderWriterMGN()
        {}

        virtual ~ReaderWriterMGN()
        {}

        // Needs to return an unmanaged osg::Group*
        // See swgOSG loadSKMG
        ReadResult readNode(std::istream& fin, const Options* options = 0) const
        {
            // Read from stream into skmg record
            ml::skmg swgSKMG;
            unsigned int size = swgSKMG.readSKMG( fin );
            
            if( 0 == size )
            {
                return NULL;
            }
            
            // Create new geode to store geometry.
            osg::ref_ptr< osg::Geode > geode(new osg::Geode());            
            
            //unsigned int numPsdt = swgSKMG.getNumPsdt();
            //
            //for(unsigned int j = 0; j < numPsdt; ++j)
            //{
            //    osg::Vec3Array* vertices = new osg::Vec3Array;
            //    osg::Vec3Array* normals = new osg::Vec3Array;
            //    osg::Vec4Array* colors = new osg::Vec4Array;
            //    osg::Vec2Array* texCoords = new osg::Vec2Array;
            //    
            //    const ml::skmg::psdt &newPsdt = swgSKMG.getPsdt( j );
            //
            //    // Build list of vertices used with current indices.
            //    float x, y, z;
            //    for( unsigned int i = 0; i < newPsdt.getNumVertex(); ++i )
            //    {
            //        newPsdt.getVertex( i, x, y, z );
            //        vertices->push_back( osg::Vec3( x, y, z ) );
            //        
            //        std::cout << "xyz: " << x << ", " << y << ", " << z << std::endl;
            //        
            //        newPsdt.getNormal( i, x, y, z );
            //        normals->push_back( osg::Vec3( x, y, z ) );
            //        
            //        // Hard code color for now
            //        colors->push_back( osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
            //        
            //        newPsdt.getTexCoord( i, x, y );
            //        texCoords->push_back( osg::Vec2( x, y ) );
            //        std::cout << "uv: " << x << ", " << y << std::endl;
            //    }
            //    
            //    // Create new geometry node list of vertex attributes.
            //    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
            //    
            //    geometry->setVertexArray( vertices );
            //    
            //    geometry->setColorArray( colors );
            //    geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
            //    
            //    geometry->setNormalArray( normals );
            //    geometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );      
            //    
            //    geometry->setTexCoordArray( 0, texCoords );
            //    
            //    std::cout << "Num groups: " << swgSKMG.getNumGroups() << std::endl;
            //    osg::ElementBufferObject* ebo = new osg::ElementBufferObject;
            //    for( unsigned short int i = 0; i <= swgSKMG.getNumGroups(); ++i )
            //    {
            //        // Create new primitive set to hold this list.
            //        osg::DrawElementsUShort* drawElements =
            //          new osg::DrawElementsUShort( osg::PrimitiveSet::TRIANGLES );
            //          
            //        const std::vector<unsigned int> &oitl = newPsdt.getOTriangles( i-1 );
            //        std::cout << "Group " << (i-1) << ": Num triangles: "
            //    	        << (oitl.size()/3) << std::endl;
            //        // Populate primitive set with indices.
            //        for( unsigned int j = 0; j < oitl.size(); ++j )
            //        {
            //            drawElements->push_back( oitl[j] );
            //        }
            //        
            //        // Add primitive set to this geometry node.
            //        drawElements->setElementBufferObject( ebo );
            //        geometry->addPrimitiveSet( drawElements );
            //    }
            //    
            //    {
            //        // Create new primitive set to hold this list.
            //        osg::DrawElementsUShort* drawElements =
            //          new osg::DrawElementsUShort( osg::PrimitiveSet::TRIANGLES );
            //        
            //        const std::vector<unsigned int> &itl = newPsdt.getTriangles();
            //        // Populate primitive set with indices.
            //        for( unsigned int j = 0; j < itl.size(); ++j )
            //        {
            //          drawElements->push_back( itl[j] );
            //        }
            //
            //        // Add primitive set to this geometry node.
            //        drawElements->setElementBufferObject( ebo );
            //        geometry->addPrimitiveSet( drawElements );
            //    }  
            //    
            //    // Load shader and attach to this geometry node.
            //    std::string shaderFilename = newPsdt.getShader();
            //    geometry->setStateSet( loadShader( shaderFilename ) );
            //    
            //    osg::VertexBufferObject *vbo = new osg::VertexBufferObject;
            //    vertices->setVertexBufferObject( vbo );
            //    
            //    geometry->setUseVertexBufferObjects( ( NULL != vbo ) );
            //    
            //    geode->addDrawable( geometry.get() );
            //}   
            
            return geode.release();
        }
        
        // Needs to return an unmanaged osg::Group*
        ReadResult readNode(const std::string& file, const Options* options = 0) const
        {
            return ReadResult();
        }
    };
    
    REGISTER_OSGPLUGIN(mgn, ReaderWriterMGN)
}
