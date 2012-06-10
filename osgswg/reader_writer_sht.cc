// Reader/Writer driver for .mgn files

#include <iostream>
#include <memory>
#include <string>

#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4250 )
#endif

#include <osg/Material>
#include <osgDB/ReaderWriter>
#include <osgDB/Registry>

#ifdef WIN32
#pragma warning( pop )
#endif

namespace osgswg {

    class ReaderWriterSHT : public osgDB::ReaderWriter
    {
    public:
        ReaderWriterSHT()
        {}

        virtual ~ReaderWriterSHT()
        {}

        // Needs to return an unmanaged osg::Group*
        // See swgOSG loadSKMG
        ReadResult readObject(std::istream& fin, const Options* options = 0) const
        {
            return ReadResult();
        }
        
        // Needs to return an unmanaged osg::Group*
        ReadResult readObject(const std::string& file, const Options* options = 0) const
        {
            osg::ref_ptr< osg::StateSet > stateSet( new osg::StateSet );
            return stateSet.release();
        }
    };
    
    REGISTER_OSGPLUGIN(sht, ReaderWriterSHT)
}
