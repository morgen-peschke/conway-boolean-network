conway-boolean-network
======================

Conway's game of life implemented as a 2D boolean network function.

About
-----

This is two projects, a simple [1D boolean
network](http://en.wikipedia.org/wiki/Boolean_network) assignment for my
Discrete Math class, and a second expansion of the concept into two dimensions
that is configured to play Conway's Game of Life.

The original project was the implementation of an [elementary cellular
automaton](http://en.wikipedia.org/wiki/Cellular_automaton#Elementary_cellular_automata),
which encoded the transition function as an integer from 0-255. The output of
the original project was two dimensional, with time as the second
dimension.

The modified project expanded the concepts into two dimensions while attempting
to maintain the same style of notating the transition function. The output is a
series of frames, which were a pain to read, so animate.pl was created to handle
scrolling the output in an automatic fashion. It is admittedly not terribly
elegant, but it gets the job done, and is far simpler that using ncurses or an
equivalent alternative.


