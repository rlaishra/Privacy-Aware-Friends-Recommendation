import csv, networkx

class Layer(object):
	"""Class representing a layer in the social network"""
	def __init__(self, file_network, file_profile, file_interlayer):
		super(Layer, self).__init__()
		self.__file_network = file_network
		self.__file_profile = file_profile
		self.__file_interlayer = file_interlayer
		self.__graph = None
		self.__profile = None
		self.__inter_layer = None

	



		