/* ************************************************************************** */
/*                                                          LE - /            */
/*                                                              /             */
/*   shell_prepare.c                                  .::    .:/ .      .::   */
/*                                                 +:+:+   +:    +:  +:+:+    */
/*   By: ythollet <ythollet@student.le-101.fr>      +:+   +:    +:    +:+     */
/*                                                 #+#   #+    #+    #+#      */
/*   Created: 2018/12/12 01:08:34 by ythollet     #+#   ##    ##    #+#       */
/*   Updated: 2018/12/12 01:08:34 by ythollet    ###    #+. /#+    ###.fr     */
/*                                                         /                  */
/*                                                        /                   */
/* ************************************************************************** */

#include "shell.h"

char	**ft_arrjoin(char **arr1, char **arr2)
{
	int		cursor_arrs;
	int		cursor_all_env;
	char	**all_env;

	cursor_arrs = 0;
	cursor_all_env = 0;
	all_env = malloc(sizeof(char *) * (ft_arrlen(arr1) + ft_arrlen(arr2) + 1));
	while (arr1[cursor_arrs])
		all_env[cursor_all_env++] = ft_strdup(arr1[cursor_arrs++]);
	cursor_arrs = 0;
	while (arr2[cursor_arrs])
		all_env[cursor_all_env++] = ft_strdup(arr2[cursor_arrs++]);
	all_env[cursor_all_env] = NULL;
	return (all_env);
}

void	shell_prepare_args(t_cmd *elem, t_shell *shell)
{
	int i;

	i = 0;
	shell_clean_emptyargs(elem);
	while (elem->args && elem->args[i])
	{
		if (!shell_envpsub(&elem->args[i], shell->envp, shell->envl) &&
				(elem->bad_substitution = 1))
			return ;
		shl_quotesub(elem->args[i]);
		if (i == 0)
		{
			check_hash_then_path(elem, shell);
			if (!elem->exec)
				elem->exec = ft_strdup("not found");
			else if ((int)elem->exec == -1)
				elem->exec = ft_strdup("directory");
			else if ((int)elem->exec == -2)
				elem->exec = ft_strdup("file or directory");
			else if ((int)elem->exec == -3)
				elem->exec = ft_strdup("no allowed");
		}
		i++;
	}
	shell_clean_emptyargs(elem);
}

int		shell_prepare_jobs_number(t_job *jobs)
{
	t_job	*job;
	int		job_num;

	job = getter_job()->first_job;
	job_num = 1;
	while ((job = job->next))
	{
		if (job->num == job_num)
		{
			job_num++;
			job = jobs;
		}
	}
	return (job_num);
}

void	shell_prepare_jobs_boucle(t_cmd *elem, t_job *first_jobs, t_job *job)
{
	t_cmd	*cpy_elem;

	while (elem)
	{
		job->next = (t_job *)malloc(sizeof(t_job));
		job = job->next;
		ft_bzero(job, sizeof(t_job));
		job->cmds = elem;
		while (elem->sep && elem->sep != PTN_VRGL && elem->sep != SPL_SPRLU)
			elem = elem->next_cmd;
		job->sep = elem->sep;
		job->num = shell_prepare_jobs_number(first_jobs);
		if (elem->next_cmd)
		{
			cpy_elem = elem;
			elem = elem->next_cmd;
			cpy_elem->next_cmd = NULL;
		}
		else
			elem = elem->next_cmd;
	}
}

/*
** cmd est découpé dans les jobs
*/

void	shell_prepare_jobs(t_job *first_jobs, t_cmd *cmd)
{
	t_job	*job;
	t_cmd	*elem;

	job = first_jobs;
	while ((job->next))
		job = job->next;
	elem = cmd->next_cmd;
	shell_prepare_jobs_boucle(elem, first_jobs, job);
	free(cmd->start);
}

/*
** Clean des arg vide et get exec path
** elem->process.stdin_send à NULL est une protection d'un seg
*/

void	shell_prepare(t_job *jobs, t_cmd *cmd)
{
	t_cmd	*elem;

	elem = cmd;
	while ((elem = elem->next_cmd))
	{
		elem->args_raw = ft_arrdup(elem->args);
		if ((int)elem->process.stdin_send == -1 ||
			(int)elem->process.stdin_send == -2)
			elem->process.stdin_send = NULL;
	}
	shell_prepare_jobs(jobs, cmd);
}
