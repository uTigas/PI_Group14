import { IonButton, IonCol, IonContent, IonFooter, IonGrid, IonHeader, IonInput, IonItem, IonList, IonMenu, IonPage, IonRow, IonSplitPane, IonTitle, IonToolbar, IonAvatar, IonLabel, IonText, IonSegmentButton, IonIcon } from '@ionic/react';
import AppAppBar from '../components/AppAppBar';
import './Chats.css';
import { useContext, useEffect, useRef, useState } from 'react';
import { format } from 'date-fns';
import { UserContext } from '../App';
import ApiWrapper from '../support/APIWrapper';
import { checkmarkOutline, closeOutline, refresh } from 'ionicons/icons';
import Invites from '../components/chat/Invites';

const Chats: React.FC = () => {
  const [activeChat, setActiveChat] = useState<any>(null);
  const [invited, setInvited] = useState<any>('');
  const [message, setMessage] = useState<any>('');
  const [contacts, setContacts] = useState<any[]>([]);
  const [messages, setMessages] = useState<any[]>([]);
  const [invites, setInvites] = useState<any[]>([]);
  const userDetails = useContext(UserContext);
  const [error, setError] = useState<boolean>(false);
  const inputRef = useRef<HTMLIonInputElement>(null)
  const scroll = useRef<HTMLSpanElement>(null);

  const inviteChat = async () => {
    if (invited != ''){
      setError(false);
      const formData = new FormData;
      formData.append('username', invited);
      const response = ApiWrapper.inviteChat(formData);
      if (await response){
        refresh()
        response.catch(
          (error) => {
            setError(true)
            console.log(error)
          }
        )
      }
      fetchContacts();
    }
    else{
      setError(true);   
    } 
  }

  const fetchContacts = () => {
    try {
      const response = ApiWrapper.fetchContacts();
      if (response) {
        response.then(
          (response) => {
            setContacts(response!.data.chats)
          }
        )
      } else {
        console.error('Response is undefined');
      }
    } catch (error) {
      console.error('Error fetching contacts:', error);
    }  }
  
  const fetchInvites = () => {
    try {
      const response = ApiWrapper.fetchChatInvites();
      if (response) {
        response.then(
          (response) => {
            setInvites(response!.data.invites)
          }
        )
      } else {
        console.error('Response is undefined');
      }
    } catch (error) {
      console.error('Error fetching invites:', error);
    }
  };
  
  const fetchChat = () => {
    try {
      const response = ApiWrapper.fetchMessages(activeChat.id);
      if (response) {
        response.then(
          async (response) => {
            const decrypted_msgs = await ApiWrapper.decryptChat(response!.data.messages, activeChat.rx_id)
            setMessages(decrypted_msgs)
          }
        )
      } else {
        console.error('Response is undefined');
      }
    } catch (error) {
      console.error('Error fetching messages:', error);
    }
  }

  const acceptInvite = async (id:string) => {
    const formData = new FormData;
    formData.append('invite',id) 
    const response = ApiWrapper.acceptChatInvite(formData)
    if(await response)
      refresh()
  }
  
  const refuseInvite = async (id:string) => {
    const formData = new FormData;
    formData.append('invite',id) 
    const response = ApiWrapper.refuseChatInvite(formData)
    if(await response)
      refresh()
  }

  const sendMessage = async () => {
    const formData = new FormData;
    if (message != ''){
      formData.append('rx_id', activeChat.rx_id)
      formData.append('message', message) 
      formData.append('chat', activeChat.id)
      const response = ApiWrapper.sendMessage(formData)
      if(await response)
        refresh()
    }
  }
  
  const refresh = () => {
    setMessage('')
    fetchContacts()
    fetchInvites()
    if (activeChat)
      fetchChat()
  }

  useEffect(()=>{
    refresh()
  },[])

  useEffect(()=>{
    scroll.current?.scrollIntoView({ behavior: 'smooth' })
  },[messages])
  
  useEffect(()=>{
    if (activeChat !== null) {
      fetchChat();
    }
  },[activeChat])

  return (
    <IonPage>
      <IonHeader>
        <AppAppBar title='Chats'/>
      </IonHeader>
      <IonContent>
        <IonSplitPane when='xs' contentId='main'>
          <IonMenu contentId='main'>
            <IonContent>
              <IonButton fill='outline' id='invites' expand='block'color='success'>Invites {invites.length > 0 ? '('+invites.length+')' : ''}</IonButton>
              <Invites invites={invites} fetchInvites={fetchInvites} fetchContacts={fetchContacts} trigger='invites'/>
              <IonList lines='full'>
                {contacts.map((contact: any) => (
                  <IonItem key={contact.id} button onClick={() => {setActiveChat(contact), setMessage('')}}>
                    <IonAvatar slot="start">
                      <img src="https://ionicframework.com/docs/img/demos/avatar.svg" alt={`Avatar of ${contact.name}`} />
                    </IonAvatar>
                    <strong>{contact.name}</strong>
                  </IonItem>
                ))}
              </IonList>
            </IonContent>
          </IonMenu>
          <div className='ion-page' id='main'>
            {activeChat ? (
              <><IonHeader>
                <IonToolbar>
                  <IonAvatar slot="start">
                      <img src="https://ionicframework.com/docs/img/demos/avatar.svg" className='ion-padding'/>
                    </IonAvatar>
                  <IonTitle>
                      {activeChat.name}
                  </IonTitle>
                </IonToolbar>
              </IonHeader>
              <IonContent color='light' className='ion-padding'>
                    {messages.map((message, index) => 
                        <div key={index} className={`message ${message.sender !== userDetails?.username ? 'received' : 'sent'}`}>
                          <div className='ChatBubble'>
                            <IonLabel>{message.message}</IonLabel>
                            <br />
                            <IonText> {format(message.ts, "HH:MM")}</IonText>
                          </div>
                        </div>
                    )}
                  <span ref={scroll}></span>
                  {/*
                  {messages.map((message: any) => (
                    <div
                      key={message.id}
                      className={`ChatBubble ${message.sender !== userDetails?.username ? 'received' : 'sent'}`}
                    >
                      <div className="MessageContent">
                        <strong>{message.message}</strong>
                        <br />
                        <IonText style={{ float: 'right' }}> {format(message.ts, "HH:MM")}</IonText>
                      </div>
                    </div>
                  ))}
                  */ }
                </IonContent><IonFooter>
                  <IonToolbar>
                    <IonGrid>
                      <IonRow>
                        <IonCol>
                          <IonInput ref={inputRef} clearInput={true} aria-label="text" placeholder='Enter text' onIonChange={(e) => setMessage(e.detail.value)} onKeyUp={(e) => {if (e.key === "Enter") {sendMessage(), inputRef.current!.value=''}}}></IonInput>
                        </IonCol>
                        <IonCol size='auto' className='ion-text-end'>
                          <IonButton fill='outline' onClick={() => {sendMessage(), inputRef.current!.value=''}}>Send</IonButton>
                        </IonCol>
                      </IonRow>
                    </IonGrid>
                  </IonToolbar>
                </IonFooter></>
            ):(<ChatMessage />)}
          </div>
        </IonSplitPane>
      </IonContent>
    </IonPage>
  );
};

const ChatMessage: React.FC = () => {
  return (
    <IonContent>
      <div className="container">
        <strong>QeeP Quantum Secure Chat System</strong>
        <p>Only Chatters can access shared data. Your messages are quantum encrypted in order to maximize the privacy and safety of your most precious data.</p>
      </div>
    </IonContent>
  )
}

export default Chats;
