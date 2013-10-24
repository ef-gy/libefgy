/*
 * This file is part of the ef.gy project.
 * See the appropriate repository at http://ef.gy/.git for exact file
 * modification records.
*/

/*
 * Copyright (c) 2012-2013, ef.gy Project Members
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#if !defined(EF_GY_GENETIC_H)
#define EF_GY_GENETIC_H

#include <vector>
#include <cstdlib>
#include <ctime>

namespace efgy {
   
   namespace optimise { 
    
    
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
             
           private:
             T genome[genomeLength];
             
    
        };

       protected:
        /// population
        std::vector<GAIndividual> population;

        /// mutation probability
        Q pMutate;

        /// crossover probability
        Q pCrossover;

        /// number of current generation
        int currentGeneration;
    

        void select()
        {
           std::vector<GAIndividual> newPopulation = Select(populationSize, population);

        }


       public:
        geneticAlgorithm(Q pMutate_, Q pCrossover_) : pMutate(pMutate_), pCrossover(pCrossover_)
        {
            for(int i = 0; i < populationSize; i++)
            {
                population.push_back(GAIndividual());
            }
        }

        void mutate(GAIndividual &individual)
        {
            std::srand(std::time(NULL));
            int position = rand() % genomeLength;
            
            individual.set(position, Mutate(individual[position]));
        }

        

    };
   }
}

#endif
