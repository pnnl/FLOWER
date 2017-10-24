%define __spec_install_post %{nil}
%define debug_package %{nil}
%define __os_install_post %{_dbpath}/brp-compress
#%define build_timestamp %(date +"%Y%m%d")

Name: boost
Version: 1.60
#Release: 0_%{build_timestamp}
Release: 0
Summary: Boost Distributution
License: GPL+
Group: System Environment/Base
Source: boost-1.60.tar
Prefix: /usr/local/boost
BuildRoot: %{_tmppath}/%{name}-%{version}-root
#Requires: boost > 1.6, gcc > 5.3

Packager: Laura Curtis <Laura.Curtis@pnnl.gov>

%description
Boost application packaged for use with Flower

%prep
%setup

%build

%install
echo ${RPM_BUILD_ROOT}
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf %{buildroot}
mkdir -p %{buildroot}%{prefix}
cp -a ${RPM_BUILD_DIR}/boost-1.60/* %{buildroot}%{prefix}

%clean
#rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{prefix}/*

%pre
# Create flower user, if needed
/usr/bin/getent passwd flower || /usr/sbin/adduser flower

%post
#chown -R flower.flower /home/flower


%changelog
* Fri Jan 13 2017 (Laura Curtis) 1.0
-- Initial Build
