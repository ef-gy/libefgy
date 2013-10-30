/**\file
 *
 * \copyright
 * Copyright (c) 2012-2013, ef.gy Project Members
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

namespace efgy
{
    namespace optimise
    {
    /*
        A class that handles classic genetic algorithms. 

        This class contains the entire population of the GA; individual genomes are 
        represented as GAIndividual<T, genomeLength>. 

        The template parameter Fitness is assumed to have a _static_ overloaded () operator that accepts
        a GAIndividual and returns a value of type Q.

        Q is assumed to be a type that represents rational numbers; the default is double,
        but one might want to use long doubles, fractions, or other rational data types instead.
        Since values of type Q are used to represent probabilities, please bear in mind
        that Q should be able to represent values between 0 and 1, so integers are not a good choice.

        Mutate is assumed to overload the () operator, accept an argument of type T and return
        a value of type T. The functor may either mutate its argument explicitly (for example, apply
        the NOT operator to a boolean) or simply return a random value of type T, but users of this class
        should assume that the functor does not return its argument unchanged.

        hasTerminated is assumed to be a functor which takes no arguments and returns a boolean value that 
        indicates whether the genetic algorithm should terminate.

        Select is assumed to be a functor that takes the target population size and a vector of GAIndividual
        as its arguments, and returns a vector of GAIndividual as the new population, implementing a selection
        method of the user's choice.

        Initialise is assumed to be a functor that takes an array of T
        and its length, initialising each element of the array
        to some kind of value of type T. A likely choice is to just
        initialise each element with a random value.

    */
    template<typename T, int genomeLength, typename Fitness, typename Mutate, typename hasTerminated, typename Select, typename Initialise, int populationSize=1000, typename Q=double>
    class geneticAlgorithm
    {


       private:
        /// an individual for genetic algorithms
    
        class GAIndividual
        {
           public:
             GAIndividual()
             {
                Initialise(genome, genomeLength);
             }
            
             void set(int position, T newValue)
             {
                if((0 <= position) && (position <= genomeLength))
                {
                    genome[position] = newValue;
                }
             }
             
            T genome[genomeLength];
             
    
        };

       protected:
        /// population
        std::vector<GAIndividual> population;

        /// mutation probability
        Q pMutate;

        /// onePointCrossover probability
        Q pCrossover;

        void breedNextGeneration()
        {
           std::vector<GAIndividual> parents = Select(populationSize, population);
           
           std::vector<GAIndividual> children;
           while(children.size() < populationSize)
           {
              std::srand(std::time(NULL));


              int p1 = (rand() / (double) RAND_MAX) * parents.size();
              int p2 = (rand() / (double) RAND_MAX) * parents.size();
              
              GAIndividual i1 = parents[p1];
              GAIndividual i2 = parents[p2];

              std::pair<GAIndividual, GAIndividual> offspring;
              double r = (double) rand() / (double) RAND_MAX;
              if(r < pCrossover) {
                offspring = onePointCrossover(i1, i2);
              }
              else
              {
                offspring = std::pair<GAIndividual, GAIndividual>(i1, i2);
              }

              r = (double) rand() / (double) RAND_MAX;
              if(r < pMutate) {
                mutate(offspring.first());
                mutate(offspring.second());
              }
              children.push_back(offspring.first());
              children.push_back(offspring.second());

           }

           population.swap(children);
        }


        void mutate(GAIndividual &individual)
        {
            std::srand(std::time(NULL));
            int position = std::rand() % genomeLength;
            
            individual.set(position, Mutate(individual[position]));
        }
       
        std::pair<GAIndividual, GAIndividual>  onePointCrossover(GAIndividual &i1, GAIndividual &i2)
        {
           int position =  (rand() / (double) RAND_MAX) * genomeLength;

           for(int k = 0; k < position; k++)
            {
                T tmp = i1.genome[k];
                i1.genome[k] = i2.genome[k];
                i2.genome[k] = tmp;
            }
           return std::pair<GAIndividual, GAIndividual> (i1, i2);
        }
        
       public:
        geneticAlgorithm(Q pMutate_, Q pCrossover_) : pMutate(pMutate_), pCrossover(pCrossover_)
        {
            for(int i = 0; i < populationSize; i++)
            {
                population.push_back(GAIndividual());
            }
        }


        GAIndividual start()
        {
            while(!hasTerminated())
              {
                breedNextGeneration();
              }

            std::map<Q, GAIndividual> lastGeneration;
            for(typename std::vector<GAIndividual>::iterator it = population.begin(); it != population.end(); it++)
            {
                lastGeneration.insert(std::pair<Q, GAIndividual> (Fitness(*it), *it));
            }

            return lastGeneration.end().second;            
        }

    };
   }
}

#endif
