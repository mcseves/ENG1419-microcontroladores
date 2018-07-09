from app import consultar_rota
import serial

#ser = serial.Serial('COM4', 9600)
rota = consultar_rota()
list_coord = rota[0]["coordenadas"]
str_rota = str(len(list_coord)) + ','

for coord in list_coord:
    str_rota += str(coord['lat']) + ',' + str(coord['lng']) + ','

str_rota += '\n'

byte_coord = str_rota.encode('utf-8')
ser.write(byte_coord)

