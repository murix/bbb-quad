apt-get purge $(dpkg --get-selections | grep deinstall | awk '{print $1}')

