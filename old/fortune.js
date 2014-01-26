
<script> function makeArray(){
this.length = makeArray.arguments.length
for (var i = 0; i < this.length; i++)
this[i + 1] = makeArray.arguments[i]

}



function randNum (num) {
var now = new Date();
var rand = Math.round(num * Math.cos(now.getTime()));
if (rand < 0) rand = - rand; if (rand == 0) rand++;
return rand;
}
var wordArray = new makeArray(
   'Act natural.<BR>Don\'t turn around.',
   'When it comes to helping others, <BR>you\'ll stop at nothing.',
   'I\'m watching you. <BR>You can\'t escape.',
   'Get your elbows off the table, <BR>or the prep cook will soon be out <BR>to teach you better manners.',
   'They say you are a true <BR>miracle worker - if you work, <BR>it is a miracle.',
   'At any time, you can marry anyone <BR>you please. Trouble is, you can\'t <BR>please anyone.',
   'You are far from lazy. <BR>You put in a hard day\'s work... <BR>every week.',
   'Inspected by Number 429',
   'The words <FONT COLOR=red><i>Watoo-Owan-Koha</i></FONT> <BR>will bring bad luck and premature death <BR>to all who read them.',
   'If you live a long life, <BR>it will be a remarkable testament <BR>to your friends\' and relatives\' <BR>self-control.',
   'Success will never change you. <BR>You\'ll always be a bastard.',
   'Are you wearing that shirt <BR>just to be funny?',
   'Avoid large, heavy objects <BR>traveling at a high rate of speed.',
   'You have a big future <BR>in food service.',
   'Everything\'s better <BR>with monosodium glutamate.',
   'Tectonic plate activity <BR>will have an unexpectedly happy result.',
   'At the rate you give people headaches, <BR>you should buy stock in Excedrin.',
   'You regard free speech <BR>not as a right <BR>but as a never-ending obligation.'
   );

function doit(document) {
//document.write (wordArray[randNum(wordArray.length)])
}
</script>

