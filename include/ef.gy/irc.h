/**\file
 * \brief asio.hpp IRC Server
 *
 * \copyright
 * Copyright (c) 2015, ef.gy Project Members
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
 * \see Project Source Code: https://github.com/ef-gy/libefgy
 */

#if !defined(EF_GY_IRC_H)
#define EF_GY_IRC_H

#include <set>
#include <map>
#include <iomanip>

#include <regex>
#include <system_error>
#include <algorithm>
#include <functional>

#include <ef.gy/server.h>

namespace efgy {
namespace net {
namespace irc {
static std::regex mask(const std::string &pattern) {
  static const std::regex special("([.()\\|]|\\[|\\])");
  static const std::regex wildmany("\\*");
  static const std::regex wildone("\\?");

  return std::regex(std::regex_replace(
      std::regex_replace(std::regex_replace(pattern, special, "\\$1"), wildmany,
                         ".*"),
      wildone, "."));
}

static bool isChannel(const std::string &name) {
  return (name[0] == '#' || name[0] == '&');
}

enum numericMessage {
  RPL_WELCOME = 1,
  RPL_UMODEIS = 221,
  RPL_WHOISUSER = 311,
  RPL_ENDOFWHOIS = 318,
  RPL_WHOISCHANNELS = 319,
  RPL_NAMREPLY = 353,
  RPL_ENDOFNAMES = 366,
  RPL_NOTOPIC = 331,
  RPL_TOPIC = 332,
  RPL_WHOREPLY = 352,
  RPL_ENDOFWHO = 315,
  RPL_BANLIST = 367,
  RPL_ENDOFBANLIST = 368,
  ERR_NEEDMOREPARAMS = 461,
  ERR_NOSUCHNICK = 401,
  ERR_NOSUCHSERVER = 402,
  ERR_UNKNOWNCOMMAND = 421,
  ERR_NOTONCHANNEL = 442,
  // ERR_ALREADYREGISTRED [sic]: that's the spelling in RFC 2812
  ERR_ALREADYREGISTRED = 462,
  ERR_NOCHANMODES = 477,
  ERR_NOOPERHOST = 491,
  ERR_UMODEUNKNOWNFLAG = 501,
  ERR_USERSDONTMATCH = 502
};

template <typename base, typename requestProcessor> class session;

template <typename session> class channel {
public:
  std::string topic;
};

namespace processor {
template <class sock> class server {
public:
  typedef session<sock, server<sock> > session;
  typedef channel<session> channel;

  bool pass(session &session, const std::string &pass) {
    switch (session.status) {
    case session::expect_pass_nick_user:
      // TODO: add password verification here
      break;
    default:
      return session.send(ERR_ALREADYREGISTRED);
    }

    return true;
  }

  bool pass(session &session, const std::vector<std::string> &param) {
    if (param.size() < 1) {
      return session.send(ERR_NEEDMOREPARAMS, {
        "PASS"
      });
    }

    return pass(session, param[0]);
  }

  bool nick(session &session, const std::string &nick) {
    session.nick = nick;

    switch (session.status) {
    case session::expect_pass_nick_user:
    case session::expect_nick_user:
      session.status = session::expect_user;
      break;
    case session::expect_nick:
      session.status = session::nominal;
      return hello(session);
    default:
      break;
    }

    return true;
  }

  bool nick(session &session, const std::vector<std::string> &param) {
    if (param.size() < 1) {
      return session.send(ERR_NEEDMOREPARAMS, {
        "NICK"
      });
    }

    return nick(session, param[0]);
  }

  bool user(session &session, const std::string &user, const std::string &mode,
            const std::string &real) {
    int nmode;

    try {
      nmode = std::stoi(mode);
    }
    catch (...) {
      nmode = 0;
    }

    switch (session.status) {
    case session::expect_pass_nick_user:
    case session::expect_nick_user:
    case session::expect_user:
      session.user = user;
      session.real = real;
      session.mode = "";
      if (nmode & 0x8) {
        session.mode += "i";
      }
      if (nmode & 0x4) {
        session.mode += "w";
      }
      break;
    default:
      return session.send(ERR_ALREADYREGISTRED);
    }

    switch (session.status) {
    case session::expect_pass_nick_user:
    case session::expect_nick_user:
      session.status = session::expect_nick;
      break;
    case session::expect_user:
      session.status = session::nominal;
      return hello(session);
    default:
      return session.send(ERR_ALREADYREGISTRED);
    }

    return true;
  }

  bool user(session &session, const std::vector<std::string> &param) {
    if (param.size() < 4) {
      return session.send(ERR_NEEDMOREPARAMS, {
        "USER"
      });
    }

    return user(session, param[0], param[1], param[3]);
  }

  bool ping(session &session, const std::vector<std::string> &param) {
    if (param.size() < 1) {
      return session.send(ERR_NEEDMOREPARAMS, {
        "PING"
      });
    }

    session.send("PONG", param);

    return true;
  }

  bool names(session &session, const std::string &channel) {
    std::string names;

    for (auto &sess : sessions) {
      if (sess->subscriptions.find(channel) != sess->subscriptions.end()) {
        names += (names == "" ? "" : " ") + sess->nick;
      }
    }

    session.send(RPL_NAMREPLY, {
      "=", channel, names
    });
    session.send(RPL_ENDOFNAMES, {
      channel, "End of NAMES list"
    });

    return true;
  }

  bool names(session &session, const std::string &channel,
             const std::string &target) {
    if (target != session.server.name) {
      session.send(ERR_NOSUCHSERVER, {
        target, "No such server"
      });

      return false;
    }

    return names(session, channel);
  }

  bool names(session &session, const std::vector<std::string> &param) {
    if (param.size() < 1) {
      return session.send(ERR_NEEDMOREPARAMS, {
        "NAMES"
      });
    }

    if (param.size() > 1) {
      return names(session, param[0], param[1]);
    }

    return names(session, param[0]);
  }

  bool topic(session &session, const std::string &channel) {
      //session.send(RPL_TOPIC, {channel, "Ain't nobody got time for that."});
    session.send(RPL_NOTOPIC, {
      channel,
    });

    return true;
  }

  bool topic(session &session, const std::string &channel,
             const std::string &newtopic) {
    if (session.subscriptions.find(channel) == session.subscriptions.end()) {
      session.send(ERR_NOTONCHANNEL, {
        channel,
      });
      return false;
    }

    return topic(session, channel);
  }

  bool topic(session &session, const std::vector<std::string> &param) {
    if (param.size() < 1) {
      return session.send(ERR_NEEDMOREPARAMS, {
        "TOPIC"
      });
    }

    if (param.size() > 1) {
      return topic(session, param[0], param[1]);
    }

    return topic(session, param[0]);
  }

  bool join(session &session, const std::string &channel) {
    session.subscriptions.insert(channel);

    for (auto &sess : sessions) {
      if (sess->subscriptions.find(channel) != sess->subscriptions.end()) {
        sess->send("JOIN", {
          channel
        },
                   session.prefix());
      }
    }

    return topic(session, channel) && names(session, channel);
  }

  bool join(session &session, const std::vector<std::string> &param) {
    if (param.size() < 1) {
      return session.send(ERR_NEEDMOREPARAMS, {
        "JOIN"
      });
    }

    std::istringstream input(param[0]);
    std::string channel;

    while (std::getline(input, channel, ',')) {
      join(session, channel);
    }

    return true;
  }

  bool part(session &session, const std::string &channel,
            const std::string &message) {
    for (auto &sess : sessions) {
      if (sess->subscriptions.find(channel) != sess->subscriptions.end()) {
        sess->send("PART", {
          channel
        },
                   session.prefix());
      }
    }

    session.subscriptions.erase(channel);

    return true;
  }

  bool part(session &session, const std::vector<std::string> &param) {
    if (param.size() < 1) {
      return session.send(ERR_NEEDMOREPARAMS, {
        "PART"
      });
    }

    std::string message = "No message";

    if (param.size() > 1) {
      message = param[1];
    }

    std::istringstream input(param[0]);
    std::string channel;

    while (std::getline(input, channel, ',')) {
      part(session, channel, message);
    }

    return true;
  }

  bool privmsg(session &session, const std::string &target,
               const std::string &message) {
    int targets = 0;

    for (auto &sess : sessions) {
      if (session.self == sess) {
        continue;
      }

      if (target == sess->nick) {
        targets++;
        sess->send("PRIVMSG", {
          target, message
        },
                   session.prefix());
      } else if (sess->subscriptions.find(target) !=
                 sess->subscriptions.end()) {
        targets++;
        sess->send("PRIVMSG", {
          target, message
        },
                   session.prefix());
      }
    }

    return true;
  }

  bool privmsg(session &session, const std::vector<std::string> &param) {
    if (param.size() < 2) {
      return session.send(ERR_NEEDMOREPARAMS, {
        "PRIVMSG"
      });
    }

    return privmsg(session, param[0], param[1]);
  }

  bool who(session &qsession, const std::string &query) {
    std::regex m = mask(query);
    std::set<std::shared_ptr<session> > matches;

    for (auto &sess : sessions) {
      for (auto &sub : sess->subscriptions) {
        if (std::regex_match(sub, m)) {
          matches.insert(sess->self);
        }
      }
      if (std::regex_match(sess->user, m)) {
        matches.insert(sess->self);
      } else if (std::regex_match(sess->real, m)) {
        matches.insert(sess->self);
      } else if (std::regex_match(sess->nick, m)) {
        matches.insert(sess->self);
      } else if (std::regex_match(sess->host, m)) {
        matches.insert(sess->self);
      }
    }

    for (auto &sess : matches) {
      std::string sub = "*";

      if (sess->subscriptions.find(query) != sess->subscriptions.end()) {
        sub = query;
      }

      qsession.send(RPL_WHOREPLY, {
        sub, sess->user, sess->host, sess->server.name, sess->nick, "H@", "0",
            sess->real
      });
    }

    qsession.send(RPL_ENDOFWHO, {
      query, "End of /WHO list"
    });

    return true;
  }

  bool who(session &session, const std::vector<std::string> &param) {
    if (param.size() < 1) {
      return session.send(ERR_NEEDMOREPARAMS, {
        "WHO"
      });
    }

    return who(session, param[0]);
  }

  bool whois(session &qsession, const std::string &query) {
    std::regex m = mask(query);
    std::set<std::shared_ptr<session> > matches;

    for (auto &sess : sessions) {
      if (std::regex_match(sess->user, m)) {
        matches.insert(sess->self);
      } else if (std::regex_match(sess->real, m)) {
        matches.insert(sess->self);
      } else if (std::regex_match(sess->nick, m)) {
        matches.insert(sess->self);
      } else if (std::regex_match(sess->host, m)) {
        matches.insert(sess->self);
      }
    }

    for (auto &sess : matches) {
      qsession.send(RPL_WHOISUSER, {
        sess->nick, sess->user, sess->host, "*", sess->real
      });

      std::string channels = "";

      for (auto &sub : sess->subscriptions) {
        channels += (channels == "" ? "" : " ") + sub;
      }

      qsession.send(RPL_WHOISCHANNELS, {
        sess->nick, channels
      });

      qsession.send(RPL_ENDOFWHOIS, {
        sess->nick
      });
    }

    if (matches.size() == 0) {
      qsession.send(ERR_NOSUCHNICK, {
        query
      });
    }

    return true;
  }

  bool whois(session &session, const std::vector<std::string> &param) {
    if (param.size() < 1) {
      return session.send(ERR_NEEDMOREPARAMS, {
        "WHOIS"
      });
    }

    return whois(session, param[0]);
  }

  bool oper(session &session, const std::string &user,
            const std::string &password) {
    //TODO: Operators could come in handy, but so far there's no need for them
    // in this basic implementation. So we just pretend that there's no O-lines,
    // ever.
    return session.send(ERR_NOOPERHOST);
  }

  bool oper(session &session, const std::vector<std::string> &param) {
    if (param.size() < 2) {
      return session.send(ERR_NEEDMOREPARAMS, {
        "OPER"
      });
    }

    return oper(session, param[0], param[1]);
  }

  bool mode(session &session, const std::string &channel,
            const std::vector<std::string> &modes) {
    static const std::regex ban("([-+]?)[^-+]*b");
    static const std::string addableModes = "irw";
    static const std::string removableModes = "iwoO";
    bool adding = true;

    // try to interpret the mode request as a user mode query first.
    if (!isChannel(channel)) {
      if (channel == session.nick) {
        if (modes.size() == 0) {
          return session.send(RPL_UMODEIS, {
            "+" + session.mode
          });
        }

        for (const auto &mode : modes) {
          for (const auto &mc : mode) {
            switch (mc) {
            case '+':
              adding = true;
              break;
            case '-':
              adding = false;
              break;
            default:
              if (adding) {
                if (addableModes.find(mc) == std::string::npos) {
                  session.send(ERR_UMODEUNKNOWNFLAG);
                } else {
                  session.addMode(mc);
                }
              } else {
                if (removableModes.find(mc) == std::string::npos) {
                  session.send(ERR_UMODEUNKNOWNFLAG);
                } else {
                  session.removeMode(mc);
                }
              }
            }
          }
        }

        return true;
      }

      return session.send(ERR_USERSDONTMATCH);
    }

    int sendcount = 0;

    for (unsigned int i = 0; i < modes.size(); i++) {
      std::smatch matches;
      if (std::regex_match(modes[i], matches, ban)) {
        session.send(RPL_ENDOFBANLIST, {
          channel
        });
        sendcount++;
      }
    }

    if (sendcount == 0) {
      session.send(ERR_NOCHANMODES, {
        channel
      });
    }

    return true;
  }

  bool mode(session &session, const std::vector<std::string> &param) {
    if (param.size() < 1) {
      return session.send(ERR_NEEDMOREPARAMS, {
        "MODE"
      });
    }

    std::string channel;
    std::vector<std::string> modes;

    for (auto &p : param) {
      if (channel == "") {
        channel = p;
      } else {
        modes.push_back(p);
      }
    }

    return mode(session, channel, modes);
  }

  bool quit(session &session) {
    forget(session);

    return true;
  }

  bool quit(session &session, const std::string &reason) {
    forget(session);

    return true;
  }

  bool quit(session &session, const std::vector<std::string> &param) {
    if (param.size() > 0) {
      return quit(session, param[0]);
    } else {
      return quit(session);
    }
  }

  bool dispatch(session &session, const std::string &command,
                const std::vector<std::string> &param) {
    if (command == "PASS") {
      pass(session, param);
    } else if (command == "NICK") {
      nick(session, param);
    } else if (command == "USER") {
      user(session, param);
    } else if (command == "PING") {
      ping(session, param);
    } else if (command == "NAMES") {
      names(session, param);
    } else if (command == "TOPIC") {
      topic(session, param);
    } else if (command == "JOIN") {
      join(session, param);
    } else if (command == "PART") {
      part(session, param);
    } else if (command == "PRIVMSG") {
      privmsg(session, param);
    } else if (command == "WHO") {
      who(session, param);
    } else if (command == "WHOIS") {
      whois(session, param);
    } else if (command == "OPER") {
      oper(session, param);
    } else if (command == "MODE") {
      mode(session, param);
    } else if (command == "QUIT") {
      if (quit(session, param)) {
        return true;
      }
    } else {
      other(session, command, param);
    }

    return true;
  }

  bool other(session &session, const std::string &command,
             const std::vector<std::string> &param) {
    session.server.log << "[" << session.prefix()
                       << "] unknown command:" << command;

    for (auto &p : param) {
      session.server.log << " :" << p;
    }

    session.server.log << "\n";

    session.send(ERR_UNKNOWNCOMMAND, {
      command
    });

    return true;
  }

  bool hello(session &session) {
    static const std::regex npr("[^A-Za-z0-9]+");

    session.host = "user/" + std::regex_replace(session.real, npr, "-");

    remember(session);

    session.send(RPL_WELCOME);

    return motd(session);
  }

  bool motd(session &session) { return true; }

  bool remember(session &session) {
    sessions.insert(session.self);

    return true;
  }

  bool forget(session &session) {
    sessions.erase(session.self);

    return true;
  }

protected:
  std::set<std::shared_ptr<session> > sessions;
  std::map<std::string, std::shared_ptr<channel> > channels;
};
}

template <typename base, typename requestProcessor = processor::server<base> >
using server = net::server<base, requestProcessor, session>;

template <typename base, typename requestProcessor> class session {
protected:
  typedef server<base, requestProcessor> serverType;

public:
  std::shared_ptr<session> self;

  enum {
    expect_pass_nick_user,
    expect_nick_user,
    expect_nick,
    expect_user,
    nominal,
    expect_pong,
    shutdown
  } status;

  typename base::socket socket;

  std::string user;
  std::string nick;
  std::string host;

  std::string prefix(void) { return nick + "!" + user + "@" + host; }

  std::string real;
  std::string mode;

  /**\brief Add a mode flag to the session
   *
   * Sets a mode flag if it wasn't set before. Then sends a notification to the
   * client about the new mode.
   *
   * \param[in] nmode The flag to set.
   *
   * \returns The return value of send() for the mode notifcation.
   */
  bool addMode(const char nmode) {
    std::set<char> modes(mode.begin(), mode.end());
    modes.insert(nmode);

    mode = std::string(modes.begin(), modes.end());

    return send("MODE", {
      nick, std::string("+") + nmode
    },
                prefix());
  }

  /**\brief Remove a mode flag to the session
   *
   * Unsets a mode flag if it wasn't set before. Then sends a notification to
   * the client about the mode change.
   *
   * \param[in] nmode The flag to unset.
   *
   * \returns The return value of send() for the mode notifcation.
   */
  bool removeMode(const char nmode) {
    std::set<char> modes(mode.begin(), mode.end());
    modes.erase(nmode);

    mode = std::string(modes.begin(), modes.end());

    return send("MODE", {
      nick, std::string("-") + nmode
    },
                prefix());
  }

  std::set<std::string> subscriptions;

  std::string content;

  serverType &server;

  session(serverType &pServer)
      : self(this), server(pServer), socket(pServer.io),
        status(expect_pass_nick_user), input(), host("unknown-host") {}

  ~session(void) {
    status = shutdown;

    try {
      socket.shutdown(base::socket::shutdown_both);
    }
    catch (std::system_error & e) {
      std::cerr << "exception while shutting down socket: " << e.what() << "\n";
    }

    try {
      socket.close();
    }
    catch (std::system_error & e) {
      std::cerr << "exception while closing socket: " << e.what() << "\n";
    }
  }

  void start() { read(); }

  bool send(const std::string &command,
            const std::vector<std::string> &params = {
  },
            std::string source = "") {
    bool have_trailing = false;

    if (source == "") {
      source = server.name;
    }

    std::stringstream send;

    send << ":" << source << " " << command;

    for (const std::string &param : params) {
      if (param != "") {
        if (have_trailing) {
          std::cerr
              << "WARNING: IRC: Ignored parameter after trailing parameter:"
              << param << "\n";
        }

        if (param.find(' ') != std::string::npos) {
          send << " :" << param;
          have_trailing = true;
          continue;
        }

        send << " " << param;
      } else {
        std::cerr << "WARNING: IRC: empty parameter in send.\n";
      }
    }

    send << "\r\n";

    std::cerr << "sent: " << send.str();

    asio::async_write(socket, asio::buffer(send.str()),
                      [&](std::error_code ec, std::size_t length) {
      handleWrite(ec);
    });

    return true;
  }

  bool send(const enum numericMessage num, std::vector<std::string> params = {
  },
            std::string source = "") {
    std::stringstream code;

    if (params.size() == 0) {
      switch (num) {
      case RPL_WELCOME:
        params.push_back("Welcome to the Internet Relay Network " + prefix());
        break;
      case ERR_ALREADYREGISTRED:
        params.push_back("Unauthorized command (already registered)");
        break;
      case ERR_NOOPERHOST:
        params.push_back("No O-lines for your host");
        break;
      case ERR_UMODEUNKNOWNFLAG:
        params.push_back("Unknown MODE flag");
        break;
      case ERR_USERSDONTMATCH:
        params.push_back("Cannot change mode for other users");
        break;
      default:
        break;
      }
    } else if (params.size() == 1) {
      switch (num) {
      case ERR_NOSUCHNICK:
        params.push_back("No such nick/channel");
        break;
      case ERR_NEEDMOREPARAMS:
        params.push_back("Not enough parameters");
        break;
      case RPL_NOTOPIC:
        params.push_back("No topic is set");
        break;
      case ERR_NOTONCHANNEL:
        params.push_back("You're not on that channel");
        break;
      case RPL_ENDOFWHOIS:
        params.push_back("End of WHOIS list");
        break;
      case RPL_ENDOFBANLIST:
        params.push_back("End of channel ban list");
        break;
      case ERR_NOCHANMODES:
        params.push_back("Channel doesn't support modes");
        break;
      case ERR_UNKNOWNCOMMAND:
        params.push_back("Unknown command");
      default:
        break;
      }
    }

    code << std::setfill('0') << std::setw(3) << long(num);

    params.insert(params.begin(), nick);

    return send(code.str(), params);
  }

protected:
  void handleRead(const std::error_code &error, size_t bytes_transferred) {
    if (status == shutdown) {
      return;
    }

    if (!error) {
      static const std::regex line(
          "(:([^ ]+) )?([A-Z]+)( ([^: \r][^ \r]*))?( ([^: \r][^ \r]*))?( ([^: "
          "\r][^ \r]*))?( ([^: \r][^ \r]*))?( ([^: \r][^ \r]*))?( ([^: \r][^ "
          "\r]*))?( ([^: \r][^ \r]*))?( ([^: \r][^ \r]*))?( ([^: \r][^ "
          "\r]*))?( ([^: \r][^ \r]*))?( ([^: \r][^ \r]*))?( ([^: \r][^ "
          "\r]*))?( ([^: \r][^ \r]*))?( ([^: \r][^ \r]*))?( :([^\r]+))? *\r?");
      std::istream is(&input);
      std::string s;
      std::smatch matches;

      std::getline(is, s);

      if (std::regex_match(s, matches, line)) {
        std::vector<std::string> param;
        for (unsigned int i : {
          5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33
        }) {
          if (matches[i] != "") {
            param.push_back(matches[i]);
          }
        }

        server.processor.dispatch(*this, matches[3], param);
      } else {
        server.log << "[" << prefix() << "] malformed message:" << s << "\n";
      }

      read();
    } else {
      server.processor.quit(*this, "read error");
      self.reset();
    }
  }

  void handleWrite(const std::error_code &error) {
    if (status == shutdown) {
      return;
    }

    if (!error) {
      //read();
    } else {
      server.processor.quit(*this, "write error");
      self.reset();
    }
  }

  void read(void) {
    switch (status) {
    case shutdown:
      break;
    default:
      asio::async_read_until(
          socket, input, "\n",
          [&](const asio::error_code & error, std::size_t bytes_transferred) {
        handleRead(error, bytes_transferred);
      });
      break;
    }
  }

  asio::streambuf input;
};
}
}
}

#endif
