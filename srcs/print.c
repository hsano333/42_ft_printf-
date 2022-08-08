/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsano </var/mail/hsano>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/02 00:40:34 by hsano             #+#    #+#             */
/*   Updated: 2022/08/09 03:09:39 by hsano            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "print.h"
#include "common.h"

static size_t	put_raw(const char *str, t_conversion *convs, ssize_t *print_size)
{
	size_t	len;

	len = convs->point - str;
	if (*print_size == PRINT_SIZE_OVER || len >= INT_MAX || *print_size >= INT_MAX - (int)len)
		*print_size = PRINT_SIZE_OVER;
	else
		*print_size += len;
	write(1, str, len);
	return (len);
}

static void	put_word(t_conversion *convs, va_list *args, ssize_t *print_size, \
								char *(*get_str)(va_list *, t_conversion *convs))
{
	char	padding;
	char	*str;
	size_t	padding_len;

	str = get_str(args, convs);
	if (!str)
		return ;
	padding_len = get_padding_len(convs, str, convs->arg_len);
	if (convs->arg_len + padding_len + *print_size >= INT_MAX)
	{
		*print_size = PRINT_SIZE_OVER;
		free(str);
		return ;
	}
	padding = ' ';
	if ((convs->flag_zero) && !convs->flag_minus && convs->precision == NONE)
		padding = '0';
	if (convs->flag_minus)
		convs->print_size = put_flag_minus(convs, str, padding_len, padding);
	else
		convs->print_size = put_except_minus(convs, str, padding_len, padding);
	if (convs->not_free == false)
		free(str);
	return ;
}

static void	swtiching_valid(t_conversion *convs)
{
	char	c;

	c = convs->conversion;
	if (c == 'c' || c == 's' || c == 'u' || c == 'p' || c == 'x' || c == 'X')
	{
		convs->flag_plus = false;
		convs->flag_space = false;
	}
	if (c == 'c' || c == 's' || c == 'u' || c == 'd' || c == 'i')
		convs->flag_sharp = false;
	if (c == 'c' || c == 's' || c == 'p')
		convs->flag_zero = false;
	if (c == 'p')
		convs->flag_sharp = true;
}

static size_t	put_converted_word(t_conversion *convs, va_list *args, ssize_t *print_size)
{
	if (convs->valid == false)
		return (write(1, convs->point, convs->size));
	convs->mem_err = true;
	swtiching_valid(convs);
	if (convs->conversion == 'c')
		put_word(convs, args, print_size, get_str_char);
	else if (convs->conversion == 's')
		put_word(convs, args, print_size, get_str_str);
	else if (convs->conversion == 'p')
		put_word(convs, args, print_size, get_str_point);
	else if (convs->conversion == 'd' || convs->conversion == 'i')
		put_word(convs, args, print_size, get_str_int_digit);
	else if (convs->conversion == 'u')
		put_word(convs, args, print_size, get_str_uint_digit);
	else if (convs->conversion == 'x')
		put_word(convs, args, print_size, get_str_int_lower_hex);
	else if (convs->conversion == 'X')
		put_word(convs, args, print_size, get_str_int_upper_hex);
	else if (convs->conversion == '%')
		put_word(convs, args, print_size, get_str_percent);
	if (*print_size == PRINT_SIZE_OVER || convs->print_size >= INT_MAX || *print_size >= INT_MAX - (int)convs->print_size)
		*print_size = PRINT_SIZE_OVER;
	else
		*print_size += (int)convs->print_size;
	return (convs->size);
}

int	print(const char *str, t_list *convs_list, va_list *args)
{
	size_t			i;
	ssize_t			print_size;
	void			(*del_convs)(void *);
	t_list			*head_convs_list;
	t_conversion	*convs;

	del_convs = (void (*)())clear_conversion;
	i = 0;
	print_size = 0;
	head_convs_list = convs_list;
	while (convs_list)
	{
		convs = (t_conversion *)convs_list->content;
		i += put_raw(&(str[i]), convs, &print_size);
		i += put_converted_word(convs, args, &print_size);
		if (convs->mem_err == true || print_size == PRINT_SIZE_OVER)
			break ;
		convs_list = convs_list->next;
	}
	if (((char *)str)[i] || !convs->mem_err || print_size != PRINT_SIZE_OVER)
		ft_putstr_fd_wrapper(&(((char *)str)[i]), 1, &print_size);
	ft_lstclear(&head_convs_list, del_convs);
	return (print_size);
}
