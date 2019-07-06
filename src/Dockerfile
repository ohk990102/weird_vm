FROM tsuro/nsjail
COPY bin /chall
COPY flag /tmp/flag

CMD /bin/sh -c "chmod 400 /tmp/flag && /usr/bin/nsjail -Ml --port 31337 --chroot / -R /tmp/flag:/flag -T /tmp --keep_caps --cwd /chall -t 30 -- ./server"

EXPOSE 1337
