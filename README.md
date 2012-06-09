swgOSG - A tool for visualizing Star Wars Galaxies client files
===============================================================

This is a fork of the xunil swgOSG project primarily meant as a learning tool for myself as well as to preserve the original code.

Below is a note left in the README.txt by the initial author.

> This program allows the visualization of some of the Star Wars
> Galaxies file formats.  Its only been tested in Linux, but should
> compile under windows with minimal problems.  If you have any problems
> with the code under windows, let me know and I'll try to fix them.
> 
> To build this code you will need to have my treLib and meshLib
> libraries.  You can find the latest versions at:
> http://home.earthlink.net/~xunil/
> 
> You will also need to have OpenSceneGraph (v2.7 or higher) installed
> which is available at http://www.openscenegraph.org/projects/osg
> 
> This code is still very much under development, but it does work for
> quite a few files currently.  Let me know if you encounter any odd
> issues or problems.
> 
> Once you have the software built, you run it by giving it two
> arguments, the directory containing all your .tre files and the file
> location inside the .tre.  For example, to view the highest level of
> detail Boba Fett model, you would use the following command line:
> 
> ./swgOSG treDirectory appearance/mesh/boba_fett_l0.mgn
> 
> One last thing, you will need to have the treLib and meshLib libraries
> in your path.  Under linux, the easiest thing to do is to just add them to your
> LD_LIBRARY_PATH environment variable:
> 
> setenv LD_LIBRARY_PATH yourpath/meshLib/lib:yourpath/treLib/lib
> 
> Or you can just copy the .so files into the same directory as the
> swgOSG executable if you have problems setting up your library path.
> 
> Once you have it up and running here are a few controls:
> Left mouse - rotates the model.
> Middle mouse - pans.
> Right mouse - zooms.
> c - screen shot.
> space bar - reset view.
> esc - exits.
> s - Toggles through various graphics statistics.
> / - Increases the level of detail.
> * - Decreases the level of detail.
> 1 - Trackball mode.
> 2 - Fly mode.
> 3 - Drive mode.
> 
> Contact Xunil at the SWGEmu forums if you have issues.
> http://www.swgemu.com/forums/index.php
> 
> -Xunil

An additional set of notes was left regarding some known issues.

> Unable to find in file archive:
> appearance/component/thm_nboo_thed_power_generator.cmp
> appearance/component/thm_nboo_thed_theed_palace.cmp
> appearance/component/frn_all_lamp_candlestick_free_s2_lit_l0.cmp
> 
> 
> Segfaults:
> appearance/component/thm_tato_fort_tusken_r0_mesh.cmp
> appearance/component/aaa_bluebolt.cmp
> 
> loadFile called with null filename:
> object/building/military/shared_outpost_starport.iff
> 
> Missing geometry:
> appearance/mesh/ackbar_l0.mgn
> 
> Not a shader. File is type: OPST
> shader/skin_body.sht
