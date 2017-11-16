#!/bin/zsh

tag="calice_conv_v0402_001"

#"beam"

subfolderlist=( "cerncomb" "cernecal" "cernhcal" "fnalcomb" "fnalecal" "fnalhcal" "desyhcal" "tent" "unknown" )


for subfolder in $subfolderlist; do

	echo "cdbadmin -s $DBInit tag folder $tag /cd_calice_v0402_${subfolder}"

	cdbadmin -s $DBInit tag folder $tag /cd_calice_v0402_${subfolder}

    done

done
