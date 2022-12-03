#!/bin/sh

# If there are no database files...
if [ ! -f /var/db/EDB ]; then
    # ...and there is a database in the init folder...
    if [ -f /usr/local/etc/isode-db-init/EDB ]; then
        # Copy the database from the database init folder.
        cp /usr/local/etc/isode-db-init/* /var/db || exit 33
    else # ...otherwise, just create an EDB file from a template.
        DSA_NAME=$(grep -oP 'mydsaname\s+.*' /usr/local/etc/isode/quiputailor | sed -e 's/^mydsaname\s\+//g')
        cat >> /var/db/EDB <<EOF
MASTER
1221127185525Z
$DSA_NAME
objectClass= top & applicationEntity & dSA & quipuDSA
$DSA_NAME
description= Entry created by default. Password is 'asdf'.
presentationAddress= TELEX+00728722+RFC-1006+03+0.0.0.0+17003
supportedApplicationContext= X500DSP & X500DAP & InternetDSP & quipuDSP
userPassword= {CRYPT}BPGE
manager= $DSA_NAME
acl= others # read # entry & others # read # default
eDBinfo= ##
quipuVersion= quipu 8.0 \231 (yourhostname) of Sat 26 Nov 2022 11:35:51 AM EST
EOF
        echo 'Created default EDB file in /var/db/EDB'
    fi
fi

# Now for the program itself. You HAVE to use a local path to quiputailor for some reason.
/usr/local/sbin/ros.quipu -t ./quiputailor
