const rp = require('request-promise');
const cheerio = require('cheerio');

const options = {
  uri: "https://www.cs.utexas.edu/events",
  transform: function (body) {
    return cheerio.load(body);
  }
}
var DETAILS = [];

//returns array of results
getEvents(options).then((result) => {
  console.log(result);
});

//main function
function getEvents(options) {

  //returns a promise because aynschronous programming is lovely
  return new Promise(function(resolve, reject) {
    rp(options).then(($) => {

      let urlTargets = [];
      let promiseArray = [];

      //gets url links from the main page (each url links to an event page, which is further scraped)
      $(".views-field-title").each((index, elem) => {
        let rawText = $(elem).html();
        urlTargets[index] = elemExtractor(rawText);
      });

      //given the urls, creates an array of options to use with request-promise
      let preparedOptions = prepareOptions(urlTargets);

      //goes through each of the url options and extracts relevant information in JSON format
      for (let i = 0; i < preparedOptions.length; i++) {

        //will eventually create an array of promise (do i need promises here? hmm)
        let tempPromise = new Promise(function(resolve, reject) {
          rp(preparedOptions[i]).then(($2) => {
            let currentObject = {};

            //get title
            $2("h1").each((index, elem) => {
              if (index === 1) {
                currentObject.title = $2(elem).text();
              }
            });

            //get description
            $2("p").each((index, elem) => {
              if (index === 0) {
                currentObject.description = $2(elem).text();
              }
              else {
                currentObject.description = currentObject.description + " " + $2(elem).text();
              }
            });

            //adjusts description to take out useless overhead
            let realDescriptionStart = currentObject.description.indexOf("\n", 2) + 6;
            currentObject.description = currentObject.description.substring(realDescriptionStart);

            //slightly redundant since currentObject.data provides start and end times LOL
            currentObject.date = $2(".date-display-single").text();
            currentObject.start = $2(".date-display-start").text();
            currentObject.end = $2(".date-display-end").text();


            //maybe look into breaking this early? but minimum time wasted so idk
            //ALSO be lazy: assume that every single event occurs within the gdc
            $2(".even").each((index, elem) => {
              let currentText = $2(elem).text();
              if (currentText.indexOf("GDC") != -1) {
                currentObject.room = currentText;
              }
            });

            //data about an event has been completed and stored in object, resolve
            resolve(currentObject);
          }).catch((error) => {
            reject(error);
          });
        });

        //add the promise to the promise array
        promiseArray[i] = tempPromise;
      }

      //once all events have been scraped and added to the array, resolve all promises and return the array of JSON stuffs
      Promise.all(promiseArray).then((result) => {
        resolve(result);
      });

    }).catch((error) => {
      console.error(error);
      reject(error);
    });
  });
}

//gets the urls needed from each page
function elemExtractor(rawText) {
  let hrefStart = rawText.indexOf("https");
  let hrefEnd = rawText.indexOf('"', hrefStart);
  return rawText.substring(hrefStart, hrefEnd);
}

//creates options for use in request-promise
function prepareOptions(urlTargets) {
  let optionArray = [];

  for (let i = 0; i < urlTargets.length; i++) {
    let options = {
      uri: urlTargets[i],
      transform: function (body) {
        return cheerio.load(body);
      }
    }

    optionArray[i] = options;
  }

  return optionArray;
}
