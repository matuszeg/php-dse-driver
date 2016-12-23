#!/bin/bash

set -ex

basedir=$(dirname $0)
tmpdir=$(dirname $(mktemp -u -t tmp.XXXXXXXXXX))

server_cert="$basedir/cassandra.crt"
server_cert_pem="$basedir/cassandra.pem"
client_cert="$basedir/driver.pem"
private_key="$basedir/driver.key"
passphrase="cassandra"

skeystore="$basedir/keystore.jks"
skstorepass="cassandra"
struststore="$basedir/truststore.jks"
ststorepass="cassandra"
ckeystore="$tmpdir/driver.keystore"
ckstorepass="cassandra"
ckeystorep12="$tmpdir/driver-keystore.p12"

rm -f "$server_cert" "$server_cert_pem" "$client_cert" \
  "$private_key" "$skeystore" \
  "$struststore" "$ckeystore" "$ckeystorep12"

keytool -genkeypair -noprompt \
  -keyalg RSA \
  -validity 36500 \
  -alias cassandra \
  -keystore "$skeystore" \
  -storepass "$skstorepass" \
  -dname "CN=Cassandra Server, OU=PHP Driver Tests, O=DataStax Inc., L=Santa Clara, ST=California, C=US"

keytool -exportcert -noprompt \
  -alias cassandra \
  -keystore "$skeystore" \
  -storepass "$skstorepass" \
  -file "$server_cert"

keytool -exportcert -noprompt \
  -rfc \
  -alias cassandra \
  -keystore "$skeystore" \
  -storepass "$skstorepass" \
  -file "$server_cert_pem"

chmod 400 "$server_cert_pem"

keytool -genkeypair -noprompt \
  -keyalg RSA \
  -validity 36500 \
  -alias driver \
  -keystore "$ckeystore" \
  -storepass "$ckstorepass" \
  -dname "CN=PHP Driver, OU=PHP Driver Tests, O=DataStax Inc., L=Santa Clara, ST=California, C=US"

keytool -exportcert -noprompt \
  -alias driver \
  -keystore "$ckeystore" \
  -storepass "$ckstorepass" \
  -file driver.crt

keytool -import -noprompt \
  -alias cassandra \
  -keystore "$struststore" \
  -storepass "$ststorepass" \
  -file driver.crt

keytool -exportcert -noprompt \
  -rfc \
  -alias driver \
  -keystore "$ckeystore" \
  -storepass "$ckstorepass" \
  -file "$client_cert"

chmod 400 "$client_cert"

keytool -importkeystore -noprompt \
  -srcalias certificatekey \
  -deststoretype PKCS12 \
  -srcalias driver \
  -srckeystore "$ckeystore" \
  -srcstorepass "$ckstorepass" \
  -storepass "$ckstorepass" \
  -destkeystore "$ckeystorep12"

openssl pkcs12 -nomacver -nocerts \
  -in "$ckeystorep12" \
  -password pass:"$ckstorepass" \
  -passout pass:"$passphrase" \
  -out "$private_key"

chmod 400 "$private_key"
