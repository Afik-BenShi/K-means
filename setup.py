from setuptools import Extension, setup

module = Extension("mykmeanssp", sources=['mykmeanssp.c'])
setup(name='mykmeanssp',
     version='1.0',
     description='Python wrapper for kmeans algorithm implemented in C',
     ext_modules=[module])