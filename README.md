bingot
======

bingot is a simple cryptocurrency implementation. The goal of this project is for me to understand cryptocurrency down to the code level. I have always been fascinated about bitcoin. However after reading so many articles, I still lack understanding of some implementation details. Although The real bitcoin code base has grown too large to study, two months ago, I found BasicCoin, a simple cryptocurrency implementation in Python, which inspired me to write bingot. 

bingot is written in C++ with Qt and cryptopp. It's not aimed to become another currency, but rather a simple implementation with good documentation that could serve as a start point for other innovative projects using blockchain. At this point, I’m still evaluating the correctness of my program. This step will take some time becuase cryptocurrency is a very dynamic system. An implementation needs to consider timing (For example, cryptocurrency assumes that the proof of work algorithm takes way longer time than does message broadcast.) and probabilities. And the implemenation has to offer fault tolerant features. Therefore static analysis won't work, the program needs to be put through experiments.

bingot lacks the following features to be used for production right now, but adding these features shouldn’t be very difficult.

no permanent storage, blockchain is maintained in memory.
no implementation for peer discovery.
no implementation for mining pool.

what is cryptocurrency (bitcoin):
-------------

I’d like to explain bitcoin in the following way

1. A currency needs (is) a network. for example if I purchase something from 
