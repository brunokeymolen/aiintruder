import os
import base64
***REMOVED***
***REMOVED***
from email.MIMEMultipart import MIMEMultipart
from email.MIMEBase import MIMEBase
from email import Encoders

def send_mail_gmail(username,password,toaddrs_list,msg_text,fromaddr=None,subject="Test mail",attachment_path_list=None):

    s = smtplib.SMTP('smtp.gmail.com:587')
    #s = smtplib.SMTP('smtp.telenet.be:587')
***REMOVED***
    s.login(username, password)
    s.set_debuglevel(1)
***REMOVED***
    sender = fromaddr
    recipients = toaddrs_list
***REMOVED***
    if fromaddr is not None:
        msg['From'] = sender
    msg['To'] = ", ".join(recipients)
    if attachment_path_list is not None:
        for each_file_path in attachment_path_list:
        ***REMOVED***
                file_name=each_file_path.split("/")[-1]
                part = MIMEBase('application', "octet-stream")
                part.set_payload(open(each_file_path, "rb").read())

                Encoders.encode_base64(part)
                part.add_header('Content-Disposition', 'attachment' ,filename=file_name)
        ***REMOVED***
            except:
                print "could not attache file"
    msg.attach(MIMEText(msg_text,'html'))
    s.sendmail(sender, recipients, msg.as_string())


tolist = ["bruno.keymolen@gmail.com"]

#loop over images, every 30 seconds in /mnt/videosecurity/intruder-new
#send all as attachment 
#move all sent to /mnt/videosecurity/intruder
#loop again

attachments = ["/mnt/videosecurity/ftp/MDAlarm_20190729-101139.jpg"]

user = base64.decodestring(e['AIU'])
pw = base64.decodestring(e['AIP'])
send_mail_gmail(user, pw, tolist, "testting from python.", "bruno.keymolen.dev@gmail.com", "Test *Intruder*")
