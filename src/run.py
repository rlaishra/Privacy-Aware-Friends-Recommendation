import network_generator as ngen
import sys

def generate_network(layer_count = 5, layer_size = 100, layer_connectivity = 0.15,\
 profile_length = 10, prefix = "t5"):
	gen = ngen.NetworkGenerator(layer_count, layer_size, profile_length,\
		layer_connectivity, prefix)
	gen.run()

if __name__ == '__main__':
	print(sys.argv)
	if sys.argv[1] == 'generate_network':
		print('ouo')
		layer_count = int(sys.argv[2])
		layer_size = int(sys.argv[3])
		layer_connectivity = float(sys.argv[4])
		profile_length = int(sys.argv[5])
		prefix = sys.argv[6]
		generate_network(layer_count, layer_size, layer_connectivity,\
		 profile_length, prefix)

