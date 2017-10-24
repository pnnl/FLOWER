%define __spec_install_post %{nil}
%define debug_package %{nil}
%define __os_install_post %{_dbpath}/brp-compress
#%define build_timestamp %(date +"%Y%m%d")

Name: gcc
Version: 5.3
#Release: 0_%{build_timestamp}
Release: 0
Summary: GCC Distributution
License: GPL+
Group: System Environment/Base
Source: gcc-5.3.tar
Prefix: /usr/local/gcc
BuildRoot: %{_tmppath}/%{name}-%{version}-root
#Requires: boost > 1.6, gcc > 5.3

Packager: Laura Curtis <Laura.Curtis@pnnl.gov>

%description
GCC application packaged for use with Flower

%prep
%setup

%build

%install
echo ${RPM_BUILD_ROOT}
[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf %{buildroot}
mkdir -p %{buildroot}%{prefix}
cp -a ${RPM_BUILD_DIR}/gcc-5.3/* %{buildroot}%{prefix}

%clean
#rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{prefix}/*

%pre
# Create flower user, if needed
#/usr/bin/getent passwd flower || /usr/sbin/adduser flower

%post
#chown -R flower.flower /home/flower

%changelog
* Fri Jan 17 2017 (Laura Curtis) 1.0
-- Initial Build
