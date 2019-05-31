#!/bin/bash


prefix="${CMAKE_INSTALL_PREFIX}"


###############################################################################
if [[ "$EUID" -ne 0 ]]; then
  echo "ERROR: This script, $0, must be run as root."
  exit -1
fi
###############################################################################


###############################################################################
#####  The contents of the systemd service file
###############################################################################
read -r -d '' SERVICE_FILE_CONTENTS << SFCONTENTS
[Unit]
Description=Flower Service
After=network.target

[Service]
Type=simple
User=root
ExecStart=${CMAKE_INSTALL_PREFIX}/bin/flower -c ${CMAKE_INSTALL_PREFIX}/conf/flower.conf --device=eth0
Restart=on-abort

[Install]
WantedBy=multi-user.target
SFCONTENTS
###############################################################################


###############################################################################
#####  The contents of the flower init.d file
###############################################################################
read -r -d '' INITD_FILE_CONTENTS << IFCONTENTS
#!/bin/bash
#
# flower
#
# chkconfig: 2345 69 16
# description:  Start up the Flower engine.

# Source function library.
. /etc/init.d/functions


RETVAL=$?
RUN_AS_USER=flower
RUN_AS_USER=root
ExecStart=${CMAKE_INSTALL_PREFIX}/bin/flower

case "$1" in
  start)
          echo $"Starting Flower"
          /bin/su - $RUN_AS_USER $ExecStart
          RETVAL=$?
          ;;
   stop)
          echo $"Stopping Flower"
          /bin/su - $RUN_AS_USER killall flower
          RETVAL=$?
          ;;
      *)
          echo $"Usage: $0 {start|stop}"
          RETVAL=-1
         ;;
esac

exit $RETVAL
IFCONTENTS
###############################################################################



echo ""
echo "Setting up flower configuration file"
echo ""


if [ ! -f "/etc/os-release" ]
then 
  release_value=`grep release  /etc/redhat-release | awk -F'release ' '{print $2}' | awk -F'.' '{print $1}'`
else
  release_value=`grep ^VERSION_ID= /etc/os-release | sed -e 's/"//g' | awk -F'=' '{print $2}' | awk -F'.' '{print $1}'`
fi
#echo "DEBUG: Operating System release value = $release_value"


flowerservice=/etc/systemd/system/flower.service
case "$release_value" in
   6) echo    "Configuring for Centos/Redhat $release_value"
      flowerservice=/etc/init.d/flower
      echo -e $INITD_FILE_CONTENTS   > $flowerservice
      ;;
   7) echo    "Configuring for Centos/Redhat $release_value"
      echo -e $SERVICE_FILE_CONTENTS > $flowerservice
      ;;
  16) echo    "Configuring for Ubuntu $release_value"
      echo -e $SERVICE_FILE_CONTENTS > $flowerservice
      ;;
  18) echo    "Configuring for Ubuntu $release_value"
      echo -e $SERVICE_FILE_CONTENTS > $flowerservice
      ;;
   *) echo    "Unknown or unsupported version of OS"
      echo    "release value is --$release_value--"
      exit -1
      ;;
esac


flowerconfig=${prefix}/conf/flower.conf


# Get current working directory to return to later on
savedir=`pwd`

# Get list of eligible network interfaces
cd /sys/class/net

declare -a netinterfaces
counter=0

for i in *; do
  if [ "$i" != "lo" ]; then
    netinterfaces[${counter}]=$i
    (( counter=counter+1 ))
  fi
done

# If not active network interface, quit since flower will not run without it
if [ "$counter" -eq 0 ]; then
  echo "No activated network interfaces available. Run create-flower-config.sh"
  echo "when network interface is available."
  exit -1
fi


cd $savedir


verified=0
while [ ! "$verified" ]; do

  # Prompt for network interface and list available interfaces.
  echo ""
  echo -n "Which interface would you like to connect to?"
  echo -n "  [ "
  for i in ${!netinterfaces[@]}; do
    echo -n "${netinterfaces[$i]}"
    echo -n " "
  done
  echo "]"

  read answer

  # Check to see if active interface selected
  if [ -d /sys/class/net/$answer ]; then
    verified=1
    break
  fi

  echo "ERROR: Valid interface not selected. Please try again."

done 
  

sed -i "s#ExecStart=.*#ExecStart=${prefix}\/bin\/flower -c ${prefix}\/conf\/flower\.conf --device=$answer#g" $flowerservice


sitename=`grep site-name $flowerconfig | awk -F'=[ \t]*' '{print $2}'`


# Prompt for sitename
echo
echo -n "Enter the name of the sitename or hit enter to take default "


if [ "$sitename" == "CHANGE_ME" ]; then
  sitename=`hostname -s | tr '[:lower:]' '[:upper:]' | cut -c-20 | awk -F'.' '{print $1}'`
fi
echo "[ $sitename ] :"
  

read answer


if [ "$answer" != "" ]; then 
  sitename=$(echo "$answer" | tr '[:lower:]' '[:upper:]' | sed -e "s/,//g")
  if [ ${#sitename} -gt 20 ]; then
    echo -n "$sitename is too long. Name will be truncated to "
    sitename=`echo $sitename | cut -c-20`
    echo "$sitename"
    echo ""
  fi
fi 

  
echo "Changing to site name - $sitename"
sed -i "s/^site-name.*/site-name = $sitename/g" $flowerconfig


echo ""
echo "Starting service ...."
case "$release_value" in
   6) echo "Starting service for Centos/Redhat $release_value"
      chkconfig flower on
      service   flower start
      ;;
   7) echo "Starting service for Centos/Redhat $release_value"
      systemctl daemon-reload
      systemctl enable flower
      systemctl start  flower
      ;;
  16) echo "Starting service for Ubuntu $release_value"
      systemctl daemon-reload
      systemctl enable flower
      systemctl start  flower
      ;;
  18) echo "Starting service for Ubuntu $release_value"
      systemctl daemon-reload
      systemctl enable flower
      systemctl start  flower
      ;;
   *) echo "ERROR: Unknown or unsupported version of OS"
      echo "       release value is --$release_value--"
      exit -1
      ;;
esac


exit 0
