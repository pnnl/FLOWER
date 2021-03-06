%define __spec_install_post %{nil}
%define debug_package %{nil}
%define __os_install_post %{_dbpath}/brp-compress
#%define build_timestamp %(date +"%Y%m%d")

Name: flower
Version: 6.1
#Release: 0_%{build_timestamp}
Release: 1
Summary: Flower distribution
License: GPL+
Group: System Environment/Base
#Source: flower-6.0.tar
Prefix: /opt/flower
BuildRoot: %{_tmppath}/%{name}-%{version}-root
#Requires: boost >= 1.6, gcc >= 5.3

Packager: Flower <flower@pnnl.gov>

%description
Flower application

%prep
#%setup

%build

%install
echo ${RPM_BUILD_ROOT}
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf %{buildroot}
mkdir -p %{buildroot}%{prefix}
cp -a ${RPM_BUILD_DIR}/%{name}-%{version}/* %{buildroot}%{prefix}
cp -a ${RPM_BUILD_DIR}/%{name}-%{version}/.??* %{buildroot}%{prefix}

%clean
#rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{prefix}/*
%{prefix}/.??*
%config(noreplace) %{prefix}/conf/flower.conf

%pre
# Create flower user, if needed
#/usr/bin/getent passwd flower || /usr/sbin/adduser flower

# If running, stop service
#systemctl stop flower

#Config file
updatedatetime=`date +%Y%m%d-%H%M`
if [ -f ${prefix}/conf/flower.conf ]
then
  /bin/cp ${prefix}/conf/flower.conf ${prefix}/conf/flower.conf.${updatedatetime}
fi

%post
chown -R root.root $RPM_INSTALL_PREFIX
#echo prefix is %{prefix}
sed -i "s#NEWPREFIX#$RPM_INSTALL_PREFIX#" $RPM_INSTALL_PREFIX/bin/create-flower-config.sh
#cp %{prefix}/.defaults/flower.sh /etc/profile.d/
#cp %{prefix}/.defaults/flower.service /etc/systemd/system/flower.service
#%{prefix}/bin/create-flower-config.sh
#systemctl daemon-reload
#systemctl start flower
echo
echo
echo
echo
echo You must run:
echo 
echo 	 $RPM_INSTALL_PREFIX/bin/create-flower-config.sh
echo
echo as root to complete the installation. 
echo If this step is skipped, you will not be running on a valid 
echo network interface.
echo 
echo
echo


%changelog
* Wed Sep 05 2018 (Laura Curtis) 6.1.1
* Fri Oct 20 2017 (Laura Curtis) 6.1
-- Bug fixes, change to use root user and changes to help with Ubuntu port
* Fri Jun 09 2017 (Laura Curtis) 6.0
-- Bug fixes and fix of version number
* Tue Mar 07 2017 (Laura Curtis) 1.0
-- Initial Build
