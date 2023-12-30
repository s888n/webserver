#!/usr/bin/env python
import random

quotes = ["You must be the change you wish to see in the world. -Mahatma Gandhi",
    "Spread love everywhere you go. Let no one ever come to you without leaving happier. -Mother Teresa",
    "The only thing we have to fear is fear itself. -Franklin D. Roosevelt",
    "Darkness cannot drive out darkness: only light can do that. Hate cannot drive out hate: only love can do that. -Martin Luther King Jr.",
    "Do one thing every day that scares you. -Eleanor Roosevelt",
    "Well done is better than well said. -Benjamin Franklin",
    "The best and most beautiful things in the world cannot be seen or even touched - they must be felt with the heart. -Helen Keller",
    "It is during our darkest moments that we must focus to see the light. -Aristotle",
    "Do not go where the path may lead, go instead where there is no path and leave a trail. -Ralph Waldo Emerson",
    "Be yourself; everyone else is already taken. -Oscar Wilde",
    "If life were predictable it would cease to be life and be without flavor. -Eleanor Roosevelt",
    "In the end, it's not the years in your life that count. It's the life in your years. -Abraham Lincoln",
    "Life is a succession of lessons which must be lived to be understood. -Ralph Waldo Emerson",
    "You will face many defeats in life, but never let yourself be defeated. -Maya Angelou",
    "Never let the fear of striking out keep you from playing the game. -Babe Ruth",
    "Life is never fair, and perhaps it is a good thing for most of us that it is not. -Oscar Wilde",
    "The only impossible journey is the one you never begin. -Tony Robbins",
    "In this life we cannot do great things. We can only do small things with great love. -Mother Teresa",
    "Only a life lived for others is a life worthwhile. -Albert Einstein",
    "The purpose of our lives is to be happy. -Dalai Lama",
    "You may say I'm a dreamer, but I'm not the only one. I hope someday you'll join us. And the world will live as one. -John Lennon",
    "You only live once, but if you do it right, once is enough. -Mae West",
    "To be yourself in a world that is constantly trying to make you something else is the greatest accomplishment. -Ralph Waldo Emerson",
    "Don't worry when you are not recognized, but strive to be worthy of recognition. -Abraham Lincoln",
    "The greatest glory in living lies not in never falling, but in rising every time we fall. -Nelson Mandela",
    "Life is really simple, but we insist on making it complicated. -Confucius",
    "May you live all the days of your life. -Jonathan Swift",
    "Life itself is the most wonderful fairy tale. -Hans Christian Andersen",
    "Do not let making a living prevent you from making a life. -John Wooden",
    "Go confidently in the direction of your dreams! Live the life you've imagined. -Henry David Thoreau",
    "Keep smiling, because life is a beautiful thing and there's so much to smile about. -Marilyn Monroe",
    "In the depth of winter, I finally learned that within me there lay an invincible summer. -Albert Camus",
    "In three words I can sum up everything I've learned about life: it goes on. -Robert Frost",
    "So we beat on, boats against the current, borne back ceaselessly into the past. -F. Scott Fitzgerald", 
    "Life is either a daring adventure or nothing. -Helen Keller"]



quote_id = random.randint(0, len(quotes)-1)
quote_text = quotes[quote_id]

print("<!DOCTYPE html>")
print("<html lang='en'>")
print("<head>")
print("<meta charset='UTF-8'>")
print("<title>Webserv - Quotes</title>")
print("</head>")
print("<body>")
print("<h1>Quote of the day</h1>")
print(f"<p>Your quote for today is:</p>")
print(f"<p><b style='color:green ' >{quote_text}</b></p>")
print("<p>Click the button below for another quote:</p>")
print(f"<button onclick='document.location.reload()'>Another quote</button>")
print("</body>")
print("</html>")
