// Reader/Writer driver for .mgn files

#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4250 )
#endif

#include <osgDB/Registry>

#ifdef WIN32
#pragma warning( pop )
#endif

namespace osgswg {

    class ReaderWriterMGN : public osgDB::ReaderWriter
    {};
    
    REGISTER_OSGPLUGIN(mgn, ReaderWriterMGN)

}
