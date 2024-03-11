#! /bin/sh

echo 'idf path:'$IDF_PATH

if [ "$#" -eq "0" ]; then
    echo "patching..."
    # move origin file
    mv $IDF_PATH/components/bt/host/nimble/nimble/nimble/host/mesh/src/prov.c $IDF_PATH/components/bt/host/nimble/nimble/nimble/host/mesh/src/prov.c.bkp
    mv $IDF_PATH/components/bt/host/nimble/nimble/nimble/host/mesh/src/proxy.c $IDF_PATH/components/bt/host/nimble/nimble/nimble/host/mesh/src/proxy.c.bkp
    mv $IDF_PATH/components/bt/host/nimble/nimble/nimble/host/mesh/src/cfg_srv.c $IDF_PATH/components/bt/host/nimble/nimble/nimble/host/mesh/src/cfg_srv.c.bkp

    mv $IDF_PATH/components/bt/host/nimble/port/include/esp_nimble_cfg.h $IDF_PATH/components/bt/host/nimble/port/include/esp_nimble_cfg.h.bkp

    # import file

    cp ./prov.c $IDF_PATH/components/bt/host/nimble/nimble/nimble/host/mesh/src/
    cp ./proxy.c $IDF_PATH/components/bt/host/nimble/nimble/nimble/host/mesh/src/
    cp ./cfg_srv.c $IDF_PATH/components/bt/host/nimble/nimble/nimble/host/mesh/src/

    cp ./esp_nimble_cfg.h $IDF_PATH/components/bt/host/nimble/port/include/

    echo 'patch success.'

elif [ "$#" -eq "1"  -a $1 = 'clean' ]; then
    # clean
    echo "clean ...."
    rm $IDF_PATH/components/bt/host/nimble/nimble/nimble/host/mesh/src/prov.c 
    rm $IDF_PATH/components/bt/host/nimble/nimble/nimble/host/mesh/src/proxy.c 
    rm $IDF_PATH/components/bt/host/nimble/nimble/nimble/host/mesh/src/cfg_srv.c 
    rm $IDF_PATH/components/bt/host/nimble/port/include/esp_nimble_cfg.h

    mv $IDF_PATH/components/bt/host/nimble/nimble/nimble/host/mesh/src/prov.c.bkp $IDF_PATH/components/bt/host/nimble/nimble/nimble/host/mesh/src/prov.c
    mv $IDF_PATH/components/bt/host/nimble/nimble/nimble/host/mesh/src/proxy.c.bkp $IDF_PATH/components/bt/host/nimble/nimble/nimble/host/mesh/src/proxy.c
    mv $IDF_PATH/components/bt/host/nimble/nimble/nimble/host/mesh/src/cfg_srv.c.bkp $IDF_PATH/components/bt/host/nimble/nimble/nimble/host/mesh/src/cfg_srv.c

    mv $IDF_PATH/components/bt/host/nimble/port/include/esp_nimble_cfg.h.bkp $IDF_PATH/components/bt/host/nimble/port/include/esp_nimble_cfg.h
    
    echo "clean done."
fi


