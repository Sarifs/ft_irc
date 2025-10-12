/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: idioumas <idioumas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/12 16:34:59 by idioumas          #+#    #+#             */
/*   Updated: 2025/10/12 21:23:01 by idioumas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

class bot
{
    public:
        //bot(const char *port,const char mdp);
        bot();
        bot(bot &cpy);
        bot(const char * adr, const unsigned int port);
        bot& operator=(bot &rhs);
        ~bot();
    private:
};