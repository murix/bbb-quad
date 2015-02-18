dpkg-query --show --showformat='${Package;-50}\t${Installed-Size}\n' | sort -k 2 -n

