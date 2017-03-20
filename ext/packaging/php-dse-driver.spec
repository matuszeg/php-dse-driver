%define distnum %(/usr/lib/rpm/redhat/dist.sh --distnum)

%if "%{php_version}" < "5.6"
%global ini_name   dse.ini
%else
%global ini_name   40-dse.ini
%endif

Name:    %{package_name}
Epoch:   1
Version: %{driver_version}
Release: 1%{?dist}
Summary: DataStax PHP DataStax Enterprise Driver

Group: Development/Tools
License: Proprietary
URL: https://github.com/datastax/php-driver-dse
Source0: %{name}-%{version}.tar.gz

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
%if %{distnum} == 5
BuildRequires: buildsys-macros >= 5
%endif
BuildRequires: dse-cpp-driver-devel >= 1.1.0
BuildRequires: php-devel > 5.2
BuildRequires: gmp-devel

Requires: php(zend-abi) = %{php_zend_api}
Requires: php(api) = %{php_core_api}
Requires: libuv >= %{libuv_version}
Requires: openssl >= 0.9.8e
Requires: krb5-libs
Requires: gmp

%description
A driver built on top of the PHP driver for Apache Cassandra, with specific
extensions for DataStax Enterprise (DSE).

%prep
%setup -qn %{name}-%{version}

cat << 'EOF' | tee  %{ini_name}
; Enable %{summary} extension module
extension=dse.so
EOF

%build
%{_bindir}/phpize
export CFLAGS=-Wno-extended-offsetof
%configure \
    --with-dse \
    --with-libdir=%{_lib} \
    --with-php-config=%{_bindir}/php-config
make %{?_smp_mflags}

%install
make install INSTALL_ROOT=%{buildroot}
install -D -m 644 %{ini_name} %{buildroot}%{php_inidir}/%{ini_name}

%clean
rm -rf %{buildroot}

%check
# make check

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%config(noreplace) %{php_inidir}/%{ini_name}
%{php_extdir}/dse.so

%changelog
* Mon Mar 20 2017 Michael Penick <michael.penick@datastax.com> - 1.0.0-1
- release
* Wed Jan 18 2017 Michael Penick <michael.penick@datastax.com> - 1.0.0rc1-1
- rc1 release
