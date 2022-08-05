/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hsano </var/mail/hsano>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/02 00:40:34 by hsano             #+#    #+#             */
/*   Updated: 2022/08/05 13:01:00 by hsano            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

size_t	put_raw(const char *str, t_conversion *convs)
{
	size_t	len;

	len = convs->point - str;
	write(1, str, len);
	return (len);
}

int	put_word(t_conversion *convs, va_list *args, char *(*get_str)(va_list *, \
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
		put_flag_minus(convs, str, padding_len, padding);
	else
		put_except_flag_minus(convs, str, padding_len, padding);
	free(str);
	return (true);
}

void	swtiching_valid(t_conversion *convs)
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
	if (c == 'p')
	{
		convs->flag_zero = false;
		convs->flag_minus = false;
		convs->flag_sharp = true;
	}
}

size_t	put_converted_word(t_conversion *convs, va_list *args, int *error)
{
	if (convs->valid == false)
		return (write(1, convs->point, convs->size));
	swtiching_valid(convs);
	if (convs->conversion == 'c')
		*error = put_word(convs, args, get_str_char);
	else if (convs->conversion == 's')
		*error = put_word(convs, args, get_str_str);
	else if (convs->conversion == 'p')
		*error = put_word(convs, args, get_str_point);
	else if (convs->conversion == 'd')
		*error = put_word(convs, args, get_str_int_digit);
	else if (convs->conversion == 'i')
		*error = put_word(convs, args, get_str_int_digit);
	else if (convs->conversion == 'u')
		*error = put_word(convs, args, get_str_uint_digit);
	else if (convs->conversion == 'x')
		*error = put_word(convs, args, get_str_int_lower_hex);
	else if (convs->conversion == 'X')
		*error = put_word(convs, args, get_str_int_upper_hex);
	else if (convs->conversion == '%')
		*error = put_word(convs, args, get_str_percent);
	return (convs->size);
}

void	print(const char *str, t_list *c_list, va_list *args)
{
	int		error;
	size_t	i;
	t_list	*tmp_list;

	error = true;
	if (c_list == NULL)
		ft_putstr_fd((char *)str, 1);
	tmp_list = c_list;
	i = 0;
	while (tmp_list)
	{
		i += put_raw(&(str[i]), tmp_list->content);
		i += put_converted_word(tmp_list->content, args, &error);
		if (error == false)
			return ;
		tmp_list = tmp_list->next;
	}
	ft_putstr_fd((char *)&(str[i]), 1);
}
