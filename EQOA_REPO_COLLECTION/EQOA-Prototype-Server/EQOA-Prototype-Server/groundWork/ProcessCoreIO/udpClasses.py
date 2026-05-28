#!/usr/bin/env python
#
# Devin and Ben 
# December 15, 2018 
#
import asyncio, logging 
#
######################################################
# from aioudp.py 
# 
# UDP Endpoint classes

class udpEndpoint:
    """High-level interface for UDP enpoints.
    It is initialized with an optional queue size for the incoming datagrams.
    """

    #def __init__(self, theQueue,queue_size=None):
    def __init__(self, incomingQueue, outgoingQueue):
        self.log       = logging.getLogger('UDP_Manager')
        #if queue_size is None:
        #    queue_size = 0
        #self._queue = asyncio.Queue(queue_size)
        self._queue = incomingQueue
        self._oqueue = outgoingQueue 
        self._closed = False
        self._transport = None

    # Protocol callbacks

    def feed_datagram(self, data, addr):
        try:
            self._queue.put_nowait((data, addr))
        except asyncio.QueueFull:
            warnings.warn('Endpoint queue is full')

    def close(self):
        # Manage flag
        if self._closed:
            return
        self._closed = True
        # Wake up
        if self._queue.empty():
            self.feed_datagram(None, None)
        # Close transport
        if self._transport:
            self._transport.close()

    # User methods - these are used yet. Might use send later

    async def send(self):
      """Send a datagram to the given address."""
      while True:
        if self._closed:
          return
        if self._oqueue.empty() == True:
          await asyncio.sleep(0)
        else:
          data, addr = await self._oqueue.get()
          self.log.info('Sending to {:}, {:}'.format(*addr))
          self._transport.sendto(data, addr)

    async def receive(self):
        """Wait for an incoming datagram and return it with
        the corresponding address.

        This method is a coroutine.
        """
        if self._queue.empty() and self._closed:
            raise IOError("Enpoint is closed")
        data, addr = await self._queue.get()
        if data is None:
            raise IOError("Enpoint is closed")
        return data, addr

    def abort(self):
        """Close the transport immediately."""
        if self._closed:
            raise IOError("Enpoint is closed")
        self._transport.abort()
        self.close()

    # Properties

    @property
    def address(self):
        """The endpoint address as a (host, port) tuple."""
        return self._transport._sock.getsockname()

    @property
    def closed(self):
        """Indicates whether the endpoint is closed or not."""
        return self._closed
#
######################################################
#   
class udpDatagramProtocol(asyncio.DatagramProtocol):

    def __init__(self,endpoint,cnum):
      self._endpoint = endpoint
      self.cnum      = cnum                # Conduit Number
      self.log       = logging.getLogger('P_CoreIO')
      self.loop      = asyncio.get_event_loop()
      self.loop.create_task(self._endpoint.send())
 
    def connection_made(self, transport):  # Initial Bind to Socket/Port 
        self._endpoint._transport = transport 
        self.address = transport.get_extra_info('sockname')
        self.log.info("        UDP IO Conduit # {} Created at IP: {} PORT: {}".format(self.cnum, *self.address))

    def connection_lost(self, exc):
        if exc is not None: 
          self.log.error('ERROR: {}'.format(exc))
          msg = 'UDP Endpoint lost the connection: {!r}'
          warnings.warn(msg.format(exc))  
        #self.log.info('UDP connnection closed') 
        self._endpoint.close()

    def error_received(self, exc):
      msg = 'UDP Endpoint received an error: {!r}'
      warnings.warn(msg.format(exc))

    def datagram_received(self, data, addr):
        self.log = logging.getLogger(
            'CoreIO Con_{}'.format(self.cnum))
        #self.log.info(' Queuing  Message from IP: {:} PORT: {:} Data: {:}'.format(*addr,data.hex()))
        self.log.info(' ')
        self.log.info(' Queuing  Message from IP: {:} PORT: {:}'.format(*addr))
        self._endpoint.feed_datagram(data,addr) # has to do with Queue 


#
