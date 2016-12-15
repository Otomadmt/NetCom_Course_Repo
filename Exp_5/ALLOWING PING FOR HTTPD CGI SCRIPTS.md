# ALLOWING PING FOR HTTPD CGI SCRIPTS

> [Source Page](https://www.furorteutonicus.eu/2015/06/10/allowing-ping-for-httpd-cgi-scripts/)

### Problem

I use thttpd to run CGI scripts. One script uses the `ping` command to check the availability of another host.

[![cgi_diagram](https://www.furorteutonicus.eu/wp-content/uploads/2015/06/CPT-internet-CGI.png)](http://www.furorteutonicus.eu/wp-content/uploads/2015/06/CPT-internet-CGI.png)

> Diagram showing how CGI handles web requests. By Pluke (2012). Public Domain. Source: [Wikimedia Commons](http://commons.wikimedia.org/wiki/File:CPT-internet-CGI.svg).

Unfortunately, SELinux does not allow raw ip packets to be created from a CGI script. Such scripts run in the `httpd_sys_script_t` security context.

You can see this when sending the output of the ping command to the remote host, e.g. from another box:

```
elinks http://your_server/your_script.cgi

```

(Or use `curl` or whatever you like instead of `elinks`.)

Then you will see something like **ping: recvmsg: Permission denied**. Basically, the ping answer (echo message) is blocked by SELinux.

After creating custom modules, the `audit.log` file shows no *denied* actions, so it is not clear how to solve this.

### Tried without avail

I have tried a few things that would make sense.

```
# enables users to execute the ping command
setsebool user_ping on

```

You can see the *denied* stuff using, e.g.:

```
 tail -n 10 /var/log/audit/audit.log | grep denied

```

Even after using `audit2allow` to create and install custom policy modules, and getting rid of all *denied* lines from the `audit.log`, SELinux still gets in the way. The **ping: recvmsg: Permission denied** error still shows up.

So, all this just allows to use the ping command. The return packets are still blocked.

### Solution

As a workaround, you can use a *permissive domain*, so SELinux is set to the permissive mode just for the `httpd_sys_script_t` security context.

Command to execute:

```
# use a permissive domain for cgi scripts
semanage permissive -a httpd_sys_script_t

```

If you still get an error like this:

```
type=AVC msg=audit(1433880351.642:270055): avc:  denied  { sigchld } for  pid=1337 comm="your_script.cgi" scontext=system_u:system_r:httpd_t:s0 tcontext=system_u:system_r:httpd_sys_script_t:s0 tclass=process

```

then you might try to wait for a while before trying again… Somehow after a night’s sleep, it suddenly worked for me : )