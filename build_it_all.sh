echo "CLEANUP"

make platform=portux clean
make platform=pi-pico clean
make platform=pi-zero clean

echo "PORTUX ########################################################"
make platform=portux
echo "PI-PICO #######################################################"
make platform=pi-pico
echo "Pi ZERO #######################################################"
make platform=pi-zero
