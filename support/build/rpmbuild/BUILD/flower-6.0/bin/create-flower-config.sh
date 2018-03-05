#!/bin/bash
#

prefix=NEWPREFIX

if [[ "$EUID" -ne 0 ]]; then
  echo "This script must be run as root."
  exit 1
fi

echo
echo 'Setting up flower configuration file'
echo

if [ ! -f /etc/os-release ]
then 
  release_value=`grep release /etc/redhat-release | awk -F'release ' '{print $2}' | awk -F'.' '{print $1}'`
else
  release_value=`grep ^VERSION_ID= /etc/os-release | sed -e 's/"//g' | awk -F'=' '{print $2}' | awk -F'.' '{print $1}'`
fi
#echo release value - $release_value 

declare -a conffiles=( ${prefix}/.defaults/flower.sh ${prefix}/.defaults/flower.service ${prefix}/.defaults/flower ${prefix}/conf/flower.conf ${prefix}/conf/flower.env )

for i in "${conffiles[@]}"
do 
  sed -i "s#PREFIX#$prefix#g" $i
done

case $release_value in
  6) echo configuring for Centos/Redhat 6
     flowerservice=/etc/init.d/flower
     cp ${prefix}/.defaults/flower $flowerservice
     ;;
  7) echo configuring for Centos/Redhat 7
      flowerservice=/etc/systemd/system/flower.service
      cp ${prefix}/.defaults/flower.service  $flowerservice
      ;;
  16) echo configuring for Ubuntu 16
      flowerservice=/etc/systemd/system/flower.service
      cp ${prefix}/.defaults/flower.service  $flowerservice
      ;;
  *) echo Unknown or unsupported version of OS
     echo release value is --$release_value--
     exit
     ;;
esac

flowerconfig=${prefix}/conf/flower.conf

# Get current working directory to return to later on
pwd=`pwd`

# Get list of eligible network interfaces
networkdir="/sys/class/net"
cd $networkdir

declare -a netinterfaces
counter=0

for i in *; do
  if [ $i != "lo" ]
  then
    netinterfaces[${counter}]=$i
    (( counter=counter+1 ))
  fi
done

# If not active network interface, quit since flower will not run without it
if [ $counter -eq 0 ]; then
  echo No activated network interfaces available. Run create-flower-config.sh
  echo when network interface is available.
  exit
fi

cd $pwd

notverified=1
while [ $notverified ]; do

  # Prompt for network interface and list available interfaces.
  echo
  echo -n "Which interface would you like to connect to? ["
  for i in ${!netinterfaces[@]}; do
    echo -n "${netinterfaces[$i]}"
    echo -n " "
  done
  echo "]"

  read answer


  # Check to see if active interface selected
  for i in ${!netinterfaces[@]}; do
    if [ "${netinterfaces[$i]}" = "$answer" ]; then
      notverified=0
      break
    fi
  done
  
  if [ "$notverified" -eq 1 ]; then
    echo Valid interface not selected. Please try again.
  else
    break
  fi

done 
  
sed -i "s#ExecStart=.*#ExecStart=${prefix}\/bin\/flower -c ${prefix}\/conf\/flower\.conf --device=$answer#g" $flowerservice


sitename=`grep site-name $flowerconfig | awk -F'=[ \t]*' '{print $2}'`

# Prompt for sitename
echo
echo -n "Enter the name of the sitename or hit enter to take default "

if [ "$sitename" == "CHANGE_ME" ]; then
  thishost=`hostname -s`
  sitetemp=$(echo "$thishost" | tr '[:lower:]' '[:upper:]')
  sitename=`echo $sitetemp | sed "s/^\(........\).*/\1/g"`
fi
echo "[ $sitename ] :"
  
read answer

if [ "$answer" != "" ]; then 
  sitename=$(echo "$answer" | tr '[:lower:]' '[:upper:]')
  if [ ${#sitename} -gt 20 ]; then
    echo -n "$sitename is too long. Name will be truncated to "
    sitetemp=`echo $sitename | sed "s/^\(....................\).*/\1/g"`
    sitename=$sitetemp
    echo $sitename
    echo ""
  fi
fi 
  
echo "Changing to site name - $sitename"
sed -i "s/^site-name.*/site-name = $sitename/g" $flowerconfig

echo ""
echo "Starting service ...."
case $release_value in
  6) echo starting service for Centos/Redhat 6
     chkconfig flower on
     service flower start
     ;;
  7) echo starting service for Centos/Redhat 7
     systemctl daemon-reload
     systemctl enable flower
     systemctl start flower
     ;;
  16) echo starting service for Ubuntu 16
     systemctl daemon-reload
     systemctl enable flower
     systemctl start flower
     ;;
  *) echo Unknown or unsupported version of OS
     echo release value is --$release_value--
     exit
     ;;
esac

