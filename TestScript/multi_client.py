# -*- coding: utf-8 -*-
__author__ = 'sm9'

import asyncore, socket
import string, random
import struct, time

HOST = '192.168.0.11'
PORT = 9001


PKT_CS_LOGIN = 1
PKT_SC_LOGIN = 2
PKT_CS_CHAT	 = 3
PKT_SC_CHAT	 = 4


def str_generator(size=128, chars=string.ascii_uppercase + string.digits):
    return ''.join(random.choice(chars) for x in range(size))


class PacketDecoder(object):

    def __init__(self):
        self._stream = ''

    def feed(self, buff):
        self._stream += buff

    def decode(self):

        while len(self._stream) > 4:
            pkt_size = struct.unpack('h', self._stream[:2])[0]
            if pkt_size > len(self._stream):
                break

            packet = self._stream[:pkt_size]
            yield packet
            self._stream = self._stream[pkt_size:]



class Client(asyncore.dispatcher):

    def __init__(self, pid):
        asyncore.dispatcher.__init__(self)
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.connect( (HOST, PORT) )
        self.decoder = PacketDecoder()
        self.islogin = False
        self.pid = pid
        self.buffer = struct.pack('hhi', 8, PKT_CS_LOGIN, pid)


    def handle_connect(self):
        pass

    def handle_close(self):
        self.close()

    def handle_read(self):
        data = self.recv(8192)
        if not data:
            #print 'NOT DATA'
            self.close()
        else:
            self.decoder.feed(data)
            for packet in self.decoder.decode():
                self.process(packet)

            if self.islogin:
                self.buffer = struct.pack('<hhi1024s', 1032, PKT_CS_CHAT, self.pid, str_generator(1000))

    def writable(self):
        return (len(self.buffer) > 0)

    def handle_write(self):
        sent = self.send(self.buffer)
        self.buffer = self.buffer[sent:]

    def process(self, packet):
        pkt_len, pkt_type = struct.unpack('hh', packet[:4])

        if pkt_type == PKT_SC_LOGIN:
            self.islogin = True
            playerId, posX, posY, posZ, playerName = struct.unpack('<iddd30s', packet[4:])
            print playerId, posX, posY, posZ, playerName

        elif pkt_type == PKT_SC_CHAT:
            playerId, playerName, chat = struct.unpack('<i30s1024s', packet[4:])
            #print playerId, playerName, chat
        else:
            print 'PKT_TYPE ERROR'


for pid in range(1000, 1100):
    client = Client(pid)
    time.sleep(0.02)

asyncore.loop()
