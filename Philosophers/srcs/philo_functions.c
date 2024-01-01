/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_functions.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgoffaux <sgoffaux@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/10 10:10:57 by sgoffaux          #+#    #+#             */
/*   Updated: 2022/09/21 11:31:05 by sgoffaux         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	philo_print(char *msg, t_philo *philo, int unlock)
{
	char	*timestamp;

	timestamp = ft_itoa(get_time() - philo->env->start_time);
	pthread_mutex_lock(&philo->env->writing);
	if (!philo->env->stop_condition && !philo->env->max_ate && msg !=NULL)
		printf("%s %s %s\n", timestamp, philo->pos_str, msg);
	if (unlock)
		pthread_mutex_unlock(&philo->env->writing);
	free(timestamp);
}

void	philo_eat(t_philo *philo)
{
	pthread_mutex_lock(&philo->env->forks[philo->ffork]);
	philo_print(NULL, philo, UNLOCK);
	pthread_mutex_lock(&philo->env->forks[philo->sfork]);
	philo_print("has taken a fork", philo, UNLOCK);
	pthread_mutex_lock(&philo->env->meal);
	philo_print("is eating", philo, UNLOCK);
	philo->last_ate = get_time();
	pthread_mutex_unlock(&philo->env->meal);
	new_sleep(philo->env->time_to_eat, philo->env);
	philo->ate_times++;
	pthread_mutex_unlock(&philo->env->forks[philo->sfork]);
	pthread_mutex_unlock(&philo->env->forks[philo->ffork]);
}

void	philo_dead(t_env *env, t_philo *philo)
{
	int	i;

	while (!env->max_ate)
	{
		i = -1;
		while (++i < env->count && !env->stop_condition)
		{
			pthread_mutex_lock(&env->meal);
			if ((int)(get_time() - philo[i].last_ate) >= env->time_to_die)
			{
				philo_print("died", &philo[i], LOCK);
				env->stop_condition = 1;
			}
			pthread_mutex_unlock(&env->meal);
		}
		if (env->stop_condition)
			break ;
		i = 0;
		while (env->eat_count_max && i < env->count
			&& philo[i].ate_times >= env->eat_count_max)
			i++;
		env->max_ate = (i == env->count);
	}
}