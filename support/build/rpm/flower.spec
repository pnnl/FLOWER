%define __spec_install_post %{nil}
%define debug_package %{nil}
%define __os_install_post %{_dbpath}/brp-compress
#%define build_timestamp %(date +"%Y%m%d")

Name: flower
Version: 6.0
#Release: 0_%{build_timestamp}
Release: 0
Summary: Flower distribution
License: GPL+
Group: System Environment/Base
Source: flower-6.0.tar
Prefix: /opt/flower
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Requires: boost >= 1.6, gcc >= 5.3

Packager: Laura Curtis <Laura.Curtis@pnnl.gov>

%description
Flower application

%prep
%setup

%build

%install
echo ${RPM_BUILD_ROOT}
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf %{buildroot}
mkdir -p %{buildroot}%{prefix}
cp -a ${RPM_BUILD_DIR}/flower-6.0/* %{buildroot}%{prefix}
cp -a ${RPM_BUILD_DIR}/flower-6.0/.??* %{buildroot}%{prefix}

%clean
#rm -rf %{buildroot}

%files
%defattr(-,flower,flower,-)
%{prefix}/*
%{prefix}/.??*
#%config %{prefix}/conf/flower.conf

%pre
# Create flower user, if needed
/usr/bin/getent passwd flower || /usr/sbin/adduser flower

# If running, stop service
#systemctl stop flower

#Config file
#updatedatetime=`date +%Y%m%d-%H%M`
#/bin/mkdir -p /home/casa/etc/oldconfig
#if [ -f /home/casa/etc/siteconfig ]
#then
  #/bin/cp /home/casa/etc/siteconfig /home/casa/etc/oldconfig/${updatedatetime}-siteconfig
#fi

%post
chown -R flower.flower %{prefix}
cp %{prefix}/.defaults/flower.sh /etc/profile.d/
cp %{prefix}/.defaults/flower.service /etc/systemd/system/flower.service
systemctl daemon-reload
systemctl start flower

%changelog
* Fri Jun 09 2017 (Laura Curtis) 6.0
-- Bug fixes and fix of version number
* Tue Mar 07 2017 (Laura Curtis) 1.0
-- Initial Build
