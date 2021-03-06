#!/bin/bash

temp=make_rootmap.$$.temp

rootlibmap() {
 ROOTMAP=$1
 SOFILE=$2
 LINKDEF=$3
 shift 3
 DEPS=$*
 if [[ -e $SOFILE && -e $LINKDEF ]]; then
     rlibmap -f -o $ROOTMAP -l $SOFILE -d $DEPS -c $LINKDEF 2>> $temp
     rm -f $temp
 fi
}

######################################################
# ShowerReco3D
rootlibmap libShowerReco3D.rootmap libShowerReco3D.so $USER_DEV_DIR/ShowerReco3D/LinkDef.h \
    libClusterRecoUtil.so libFANN.so libMcshowerLookback.so libCMToolApp.so libAnalysisAlg.so













