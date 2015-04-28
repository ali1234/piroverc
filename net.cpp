/* net.c -- transmit controls over udp
 *
 * Copyright (C) 2015 Alistair Buxton <a.j.buxton@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <glib.h>
#include <gio/gio.h>

#include "control.h"
#include "net.h"

static GSocket *socket;

static gboolean send_controls(gpointer unused, gboolean ignored)
{
    char buf[12];
    GError *err = NULL;

    control_get_packet(buf);
    g_print("%02x%02x%02x%02x%02x%02x%02x%02x\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
    g_socket_send(socket, buf, 12, NULL, &err);
    return TRUE;
}

void net_start(void)
{
    GInetAddress *udpAddress;
    GSocketAddress *udpSocketAddress;
    GError *err = NULL;

    socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_DATAGRAM, G_SOCKET_PROTOCOL_UDP, &err);
    g_assert(err == NULL);

    udpAddress = g_inet_address_new_from_string("192.168.0.165");
    udpSocketAddress = g_inet_socket_address_new(udpAddress, 5005);

    g_socket_connect(socket, udpSocketAddress, NULL, &err);
    g_assert(err == NULL);

    g_timeout_add(100, (GSourceFunc)send_controls, NULL);
}

void net_stop(void)
{
    g_object_unref(socket);
}
