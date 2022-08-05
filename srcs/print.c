/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsano </var/mail/hsano>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/02 00:40:34 by hsano             #+#    #+#             */
/*   Updated: 2022/08/06 02:17:13 by hsano            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include "print.h"
#include "common.h"

static size_t	put_raw(const char *str, t_conversion *convs)
{
	size_t	len;

	len = convs->point - str;
	write(1, str, len);
	return (len);
}

static int	put_word(t_conversion *convs, va_list *args, char *(*get_str)(va_list *, \
		   	t_conversion *convs))
{
	char	padding;
	char	*str;
	int		padding_len;
	int		str_len;

	str = get_str(args, convs);
	if (!str)
		return (-1);
	str_len = ft_strlen(str);
	padding_len = get_padding_len(convs, str, str_len);
	padding = ' ';
	if ((convs->flag_zero) && !convs->flag_minus)
		padding = '0';
	if (convs->flag_minus)
		convs->print_size = put_flag_minus(convs, str, padding_len, padding);
	else
		convs->print_size = put_except_minus(convs, str, padding_len, padding);
	free(str);
	return (true);
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
	{
		convs->flag_minus = false;
		convs->flag_sharp = true;
	}
}

static size_t	put_converted_word(t_conversion *convs, va_list *args)
{
	if (convs->valid == false)
		return (write(1, convs->point, convs->size));
	swtiching_valid(convs);
	if (convs->conversion == 'c')
		put_word(convs, args, get_str_char);
	else if (convs->conversion == 's')
		put_word(convs, args, get_str_str);
	else if (convs->conversion == 'p')
		put_word(convs, args, get_str_point);
	else if (convs->conversion == 'd')
		put_word(convs, args, get_str_int_digit);
	else if (convs->conversion == 'i')
		put_word(convs, args, get_str_int_digit);
	else if (convs->conversion == 'u')
		put_word(convs, args, get_str_uint_digit);
	else if (convs->conversion == 'x')
		put_word(convs, args, get_str_int_lower_hex);
	else if (convs->conversion == 'X')
		put_word(convs, args, get_str_int_upper_hex);
	else if (convs->conversion == '%')
		put_word(convs, args, get_str_percent);
	return (convs->size);
}

int	print(const char *str, t_list *convs_list, va_list *args)
{
	size_t			i;
	int				print_size;
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
		print_size += convs->point - str - i;
		i += put_raw(&(str[i]), convs);
		i += put_converted_word(convs, args);
		if (convs->mem_err == true)
			break ;
		print_size += convs->print_size;
		convs_list = convs_list->next;
	}
	if (((char *)str)[i])
		print_size += ft_putstr_fd_wrapper(&(((char *)str)[i]), 1);
	ft_lstclear(&head_convs_list, del_convs);
	return (print_size);
}

