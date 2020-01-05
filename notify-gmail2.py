#based on: https://stackoverflow.com/questions/3362600/how-to-send-email-attachments 

import smtplib
from os.path import basename
from email.mime.application import MIMEApplication
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.utils import COMMASPACE, formatdate
import sys, getopt

def send_mail(send_from, send_to, subject, text, files=None):
    assert isinstance(send_to, list)

    msg = MIMEMultipart()
    msg['From'] = send_from
    msg['To'] = COMMASPACE.join(send_to)
    msg['Date'] = formatdate(localtime=True)
    msg['Subject'] = subject

    msg.attach(MIMEText(text))

    for f in files or []:
        with open(f, "rb") as fil:
            part = MIMEApplication(
                fil.read(),
                Name=basename(f)
            )
        # After the file is closed
        part['Content-Disposition'] = 'attachment; filename="%s"' % basename(f)
        msg.attach(part)

    
    s = smtplib.SMTP('smtp.gmail.com', 587)
    s.starttls()
    s.ehlo()

    username='@gmail.com'
    password=''
    s.login(username,password)
    
    s.sendmail(send_from, send_to, msg.as_string())
    rslt=s.quit()
    print('Sendmail result=' + str(rslt[1]))


def main(argv):
    files = []
    imgfile=''
    try:
        opts, args = getopt.getopt(argv, "f:h",["file="])
    except getopt.GetoptError:
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-f':
            imgfile = arg

    files.append(imgfile)
    to=["@somemail.com"]
    send_mail("@somemail.com", to, "Intruder detected", "Intruder seen", files)

if __name__ == "__main__":
    main(sys.argv[1:])

