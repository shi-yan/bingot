![bingot](bingot.png)
======

bingot is a simple cryptocurrency implementation. The goal of this project is for me to understand cryptocurrency down to the code level. I have always been fascinated about bitcoin. However after reading so many articles, I still lack understanding of some implementation details. Although the real bitcoin code base has grown too large to study, two months ago, I found [BasicCoin](https://github.com/zack-bitcoin/basiccoin), a simple cryptocurrency implementation in Python, which inspired me to write bingot. 

bingot is written in C++ with Qt and cryptopp. It's not aimed at becoming another currency, but rather a simple implementation with good documentation that could serve as a start point for other innovative projects using blockchain. At this point, I’m still evaluating the correctness of my program. This step will take some time because cryptocurrency is a very dynamic system. An implementation needs to consider timing (For example, cryptocurrency assumes that the proof of work algorithm takes way longer time than does message broadcast.) and probabilities. And the implementation has to offer fault tolerant features. Therefore static analysis won't work, the program needs to be put through experiments.

bingot lacks the following features to be used for production right now, but adding these features shouldn’t be very difficult.

+ no permanent storage, blockchain is maintained in memory.
+ no implementation for peer discovery.
+ no implementation for mining pool.

What is cryptocurrency (bitcoin):
-------------

I’d like to explain bitcoin in the following way:

1. A currency needs (is) a network. For example, if I purchase something from someone, the moment I hand over my money, I'm having a relationship with that person. After all the physical distance between us needs to be small enough to pass the money.

2. Bitcoin therefore is also a network. It is more similar to Facebook, where each one is connected with few others. Through them he/she can reach the whole world.

3. In the Bitcoin world, everybody keeps a ledger that has records of all transactions ever since the first second when Bitcoin started.

4. Owning 5 bitcoins in the bitcoin world means that on other people's ledgers, there is one record saying that I have 5 bitcoins. As long as most people in the world have this record, I can confidently say I indeed have 5 bitcoins.

5. Now if I want to give this money to another person, in the real world, it could be a deal only between us. No one else needs to know. But in the bitcoin world, I need to yell out load to the whole world. People who have heard my message will correct their ledgers by crossing my 5 bitcoins and adding them to that person. As long as the majority of the world has done this change, the transaction has confirmed.

6. The problem is, it takes some time for the entire world to hear my message. I could use this to favor myself. I could tell part of the world that I want to transfer 5 bitcoins, which are all the money I have, to A, and then tell the rest of the world that all my money will go to person B. This double spending trick confuses the world.

7. Therefore, there needs to be a judge, so that the rest of us can all agree on his judgements on conflicts. But who is qualified to be the judge? In the real world, such a person could be elected. But this democratic practice isn't suitable for bitcoin, because voting is inefficient, the judge can go corrupted or be controlled. After all, the core feature of bitcoin is decentralization. A single authority will certainly break this.

8. So bitcoin designed a really smart way to solve this problem. It introduces a lottery system. Everyone in the bitcoin world is drawing lottery tickets hoping to get a lucky number. Once someone gets the lucky number, he/she will play the judge and make decisions for all transactions of the past 10 minutes. All others will verify his/her lucky number and accept his/her judgements. The game then continues; Another judge will come out for the next 10 minute round. As a reward for being lucky, a judge can get certain number of bitcoins. The process of drawing lottery tickets and being judge is called mining. This is how bitcoins are generated.

9. The lottery system is mathematically tuned such that for every 10 minutes, usually there is only one lucky judge, no matter how many people are involved in the bitcoin game. The situation of having more than one judge for the same round is very rear.

10. Since one can get rewarded only by being the judge, when there are a lot of people involved in the game, the chance for each single person to be the judge is very low. This hurts the motivation of mining bitcoin. Therefore as an addition, bitcoin mining group are formed. The work of drawing lottery tickets is distributed among members of a group. Once a lucky member is rewarded, the group will redistribute the benefits between all members by contribution. This guarantees the mining payback is proportional to mining time spent, thus stimulates the mining activity. 

11. Such a lottery system is more robust than the election way or any other game rules. For one thing, the chance of me doing transactions and being the judge for the same round is very low. This reduces the incentive of becoming corrupted. For another thing, the shifting of the authority in the system offers the immunity to government intervention as opposite to a centralized system which can be easily controlled. Everyone in the bitcoin world is equally unimportant. Losing anyone won't affect the healthy of the system at all.