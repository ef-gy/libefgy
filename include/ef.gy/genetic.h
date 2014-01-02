/**\file
 *
 * \copyright
 * Copyright (c) 2012-2014, ef.gy Project Members
 * \copyright
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * \copyright
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * \copyright
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * \see Project Documentation: http://ef.gy/documentation/libefgy
 * \see Project Source Code: http://git.becquerel.org/jyujin/libefgy.git
 */

#if !defined(EF_GY_GENETIC_H)
#define EF_GY_GENETIC_H

#include <vector>
#include <cstdlib>
#include <ctime>
#include <map>
#include <array>
#include <random>

namespace efgy
{
    namespace optimise
    {
       template<typename T, int genomeLength, typename Fitness, typename Mutate, typename hasTerminated, template <typename S> class Select, typename Initialise, int populationSize=1000, typename Q=double>
       class geneticAlgorithm;

       /* \brief a Select functor that does nothing */
       template<typename S>
       class selectNull;
       
    /*
        A class that handles classic genetic algorithms. 

        This class contains the entire population of the GA; individual genomes are 
        represented as individual<T, genomeLength>. 

        \tparam Fitness is assumed to have a _static_ overloaded () operator that accepts
        a individual and returns a value of type Q.

        \tparam Q is assumed to be a type that represents rational numbers; the default is double,
        but one might want to use long doubles, fractions, or other rational data types instead.
        Since values of type Q are used to represent probabilities, please bear in mind
        that Q should be able to represent values between 0 and 1, so integers are not a good choice.

        \tparam Mutate is assumed to be a functor with a static overloaded () operator that accepts an argument of type T and return
        a value of type T. The functor may either mutate its argument explicitly (for example, apply
        the NOT operator to a boolean) or simply return a random value of type T, but users of this class
        should assume that the functor does not return its argument unchanged.

        \tparam hasTerminated is assumed to be a functor with a static overloaded () operator 
         which takes no arguments and returns a boolean value that 
        indicates whether the genetic algorithm should terminate.

        \tparam Select is assumed to be a functor with a static overloaded () operator that takes the target population size,
        a vector of individual and the Fitness functor as its arguments, 
        and returns a vector of individual as the new population, implementing a selection
        method of the user's choice. Its template argument is an instance of type geneticAlgorithm,
        usually the this pointer.

        \tparam Initialise is assumed to be a functor that takes an array of T
        and its length, initialising each element of the array
        to some kind of value of type T. A likely choice is to just
        initialise each element with a random value.

    */
    template<typename T, int genomeLength, typename Fitness, typename Mutate, typename hasTerminated, template <typename S> class Select, typename Initialise, int populationSize, typename Q>
    class geneticAlgorithm
    {
       

       protected:
        typedef Fitness FitnessFunctor;

       /// an individual for genetic algorithms
        typedef std::array<T, genomeLength> individual;
        /// population
        std::vector<individual > population;

        /// mutation probability
        Q pMutate;

        /// onePointCrossover probability
        Q pCrossover;

        std::mt19937 rng; 

        void breedNextGeneration()
        {
           Select<geneticAlgorithm<T, genomeLength, Fitness, Mutate,  hasTerminated, selectNull, Initialise, populationSize, Q > > sel(&this);
           std::vector<individual > parents = sel(populationSize, population);
           
           std::vector<individual > children;
           while(children.size() < populationSize)
           {
  
              std::uniform_int_distribution<> dis(0, parents.size()); 
              int p1 = dis(rng);
              int p2 = dis(rng);
              
              individual  i1 = parents[p1];
              individual  i2 = parents[p2];

              std::pair<individual , individual> offspring;
              double r = std::generate_canonical<double, 10>(rng);
              if(r < pCrossover) {
                offspring = onePointCrossover(i1, i2);
              }
              else
              {
                offspring = std::pair<individual , individual>(i1, i2);
              }

              r = std::generate_canonical<double, 10>(rng);
              if(r < pMutate) {
                mutate(offspring.first());
                mutate(offspring.second());
              }
              children.push_back(offspring.first());
              children.push_back(offspring.second());

           }

           population.swap(children);
        }


        void mutate(individual  &individual)
        {
           std::uniform_int_distribution<> dis(0, genomeLength);
           int position = dis(rng);
            
           individual.set(position, Mutate(individual[position]));
        }
       
        std::pair<individual , individual>  onePointCrossover(individual &i1, individual &i2)
        {
           std::uniform_int_distribution<> dis(0, genomeLength);
           int position = dis(rng);

           for(int k = 0; k < position; k++)
            {
                T tmp = i1.genome[k];
                i1.genome[k] = i2.genome[k];
                i2.genome[k] = tmp;
            }
           return std::pair<individual , individual> (i1, i2);
        }
        
       public:
        geneticAlgorithm(Q pMutate_, Q pCrossover_) : pMutate(pMutate_), pCrossover(pCrossover_)
        {
            for(int i = 0; i < populationSize; i++)
            {
                population.push_back(individual ());
            }
            std::random_device rd;
            rng = std::mt19937(rd());
             
        }


        individual  start()
        {
            while(!hasTerminated())
              {
                breedNextGeneration();
              }

            std::map<Q, individual > lastGeneration;
            for(typename std::vector<individual >::iterator it = population.begin(); it != population.end(); it++)
            {
                lastGeneration.insert(std::pair<Q, individual > (Fitness(*it), *it));
            }

            return lastGeneration.end().second;            
        }

    };

   

    /** \brief Functor that returns a random float between 0 and 1 */
    class MutateFloat
    {
        public:
            float operator () (float x) {
               std::random_device rd;
               std::mt19937 rng(rd()); 
               return std::generate_canonical<float, 10>(rng);
            }

            
    };


    
    /** \brief Implements the tournament selection method for genetic algorithms.

    */
    template<typename S, int tournamentSize>
    class SelectTournament
    {
        public:
            SelectTournament(S &s_) : GA(s_) 
            {
            }

            std::vector<typename S::individual > operator() (int targetSize, std::vector<typename S::individual > population)
            {
               std::random_device rd;
               std::mt19937 rng(rd());
               std::map<typename S::Q, typename S::individual > current;
               for(typename std::vector<typename S::individual >::iterator it = population.begin(); it != population.end(); it++)
               {
                    current.insert(std::pair<typename S::Q, typename S::individual > (typename S::FitnessFunctor(*it), *it));
               }
               
               std::vector<typename S::individual > newPopulation; 

               while (newPopulation.size () < targetSize)
               {
                    std::map<typename S::Q, typename S::individual > tournament;
                    for(int i = 0; i < tournamentSize; i++)
                    {
                        std::uniform_int_distribution<> dis(0, tournament.size());
                        int r = dis(rng);
                    {
                        typename std::map<typename S::Q, typename S::individual >::iterator it = current.begin();
                        for(int k = 0;
                            (it != current.end) && (k < r);
                            k++, it++)
                        {
                            std::pair<typename S::Q, typename S::individual> p = *it;
                            tournament.insert(p);
                        }
                    }

                    for(int i = 0; i < tournamentSize; i++)
                    {
                        typename std::map<typename S::Q, typename S::individual >::iterator it;
                        it = tournament.upper_bound(std::numeric_limits<typename S::Q>::max());
                        if(newPopulation.size < targetSize)
                        {
                            newPopulation.push_back(it->second);
                        }
                    }
               }

            }
            GA.population.swap(newPopulation);
           }

            private:
                S& GA;
    };

    /** \brief Implements GA termination after N generations.

        \tparam N is the number of generations after which the GA should terminate. 
    */
    template <int N>
    class terminateGenerations
    {
        public:
            terminateGenerations() : current(0) {};
            bool operator() (void)
            {
                return (++current > N); 
            }
        private:
            int current;
    };

    /** \brief Functor that initialises an array of float with randomly generated numbers between 0 and 1.
    
    */
   class initialiseFloatsRandomly
   {
        public:
            initialiseFloatsRandomly() {
               
            };

            void operator() (float* array, int length) 
            {
               std::random_device rd;
               std::mt19937 rng(rd()); 
               for(int i = 0; i < length; i++) {
                 float r = std::generate_canonical<float, 10>(rng); 
                 array[i] = r;
               }
            }


   };

    /** \brief Functor that initialises an array of booleans randomly..*/
    class initialiseBooleansRandomly
    {
        public:
            initialiseBooleansRandomly() {};

            void operator() (bool *array, int length)
            {
               std::random_device rd;
               std::mt19937 rng(rd);
               std::uniform_int_distribution<> dis(0, 1);
               for(int i = 0; i < length; i++) {
                  bool r = (dis(rng) % 2) == 0;
                  array [i] = r; 
               }
               
            }
    };

   }
}

#endif
