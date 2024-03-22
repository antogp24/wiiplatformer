#!/bin/sh

rm -f tags
ctags /opt/devkitpro/libogc/include/
ctags -a /opt/devkitpro/portlibs/wii/include/
ctags -a /opt/devkitpro/portlibs/ppc/include/
ctags -a /opt/devkitpro/devkitPPC/include/
ctags -a /opt/devkitpro/grr/GRRLIB/lib/
ctags -a /opt/devkitpro/grr/GRRLIB/GRRLIB/
ctags -a ./source
