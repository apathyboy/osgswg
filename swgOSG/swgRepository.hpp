/** -*-c++-*-
 *  \file   swgRepository.hpp
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

#include <iostream>
#include <string>
#include <map>
#include <memory>

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/LOD>
#include <osg/Material>
#include <osg/Math>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/StateSet>
#include <osg/Texture2D>

#include <osgAnimation/Bone>
#include <osgAnimation/Skeleton>
//#include <osgAnimation/RigGeometry>
//#include <osgAnimation/Skinning>
#include <osgAnimation/BasicAnimationManager>


#include <osgDB/ReaderWriter>

#include <treLib/treArchive.hpp>

#ifndef SWGREPOSITORY_HPP
#define SWGREPOSITORY_HPP

class swgRepository {
public:
    swgRepository(const std::string& archiveFilePath);
    ~swgRepository();

    osg::ref_ptr<osg::StateSet>          loadShader(const std::string& shaderFilename);
    osg::ref_ptr<osg::Node>              loadAPT(std::shared_ptr<std::istream> iffFile);
    osg::ref_ptr<osg::Node>              loadCMP(std::shared_ptr<std::istream> iffFile);
    osg::ref_ptr<osg::Node>              loadLOD(std::shared_ptr<std::istream> iffFile);
    osg::ref_ptr<osg::Node>              loadINLY(std::shared_ptr<std::istream> iffFile);
    osg::ref_ptr<osg::Node>              loadMSH(std::shared_ptr<std::istream> iffFile);
    osg::ref_ptr<osg::Node>              loadMLOD(std::shared_ptr<std::istream> iffFile);
    osg::ref_ptr<osg::Node>              loadPRTO(std::shared_ptr<std::istream> iffFile);
    osg::ref_ptr<osg::Node>              loadSBOT(std::shared_ptr<std::istream> iffFile);
    osg::ref_ptr<osg::Node>              loadSKMG(std::shared_ptr<std::istream> iffFile);
    osg::ref_ptr<osg::Node>              loadSTAT(std::shared_ptr<std::istream> iffFile);
    osg::ref_ptr<osg::Node>              loadSTOT(std::shared_ptr<std::istream> iffFile);
    osg::ref_ptr<osg::Node>              loadTRN(std::shared_ptr<std::istream> iffFile);
    osg::ref_ptr<osg::Node>              loadWSNP(std::shared_ptr<std::istream> iffFile);
    osg::ref_ptr<osgAnimation::Skeleton> loadSKTM(std::shared_ptr<std::istream> iffFile);

    osg::ref_ptr<osg::Node>      loadFile(const std::string& filename);
    osg::ref_ptr<osg::Texture2D> loadTextureFile(const std::string& filename);


    void createArchive(const std::string& basePath);


protected:
    osgDB::ReaderWriter*                                ddsPlugin;
    treArchive                                          archive;
    std::map<std::string, osg::ref_ptr<osg::Texture2D>> textureMap;
    std::map<std::string, osg::ref_ptr<osg::Material>>  materialMap;
    std::map<std::string, osg::ref_ptr<osg::StateSet>>  stateMap;
    std::map<std::string, osg::ref_ptr<osg::Node>>      nodeMap;
};

#endif
